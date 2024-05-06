#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libudev.h>

#include <SDL.h>

static int exit_main_loop = 0;

const char * vendor_product_id(uint16_t index)
{
    static char idstring[32];
    struct udev *udev = udev_new();
    if (!udev) {
        return NULL;
    }

    char devname[32];
    snprintf(devname, sizeof(devname), "js%u", index);

    struct udev_device *dev = udev_device_new_from_subsystem_sysname(udev, "input", devname);
    if (dev) {
        dev = udev_device_get_parent_with_subsystem_devtype(dev, "usb", "usb_device");
        if (dev) {
            const char *vendor = udev_device_get_sysattr_value(dev, "idVendor");
            const char *product = udev_device_get_sysattr_value(dev, "idProduct");
            snprintf(idstring, sizeof(idstring) - 1, "%s.%s", vendor, product);
        }
        udev_device_unref(dev);
    }

    udev_unref(udev);

    return idstring;
}

static void sigint_handler(int s)
{
    fprintf(stderr, "Caught SIGINT, exiting...\n");
    exit_main_loop = 1;
}

static void main_loop()
{
    char buffer[128];
	int joystickCount = SDL_NumJoysticks();
    SDL_Joystick* joysticks[joystickCount];
    u_int32_t inputStates[joystickCount];

    fprintf(stderr, "Initialized with %d joysticks\n", joystickCount);
	for (int i = 0; i < joystickCount; i++) {
		joysticks[i] = SDL_JoystickOpen(i);
        fprintf(stderr, "%d (%s) button count: %d\n", 
            i, 
            vendor_product_id(i),
            SDL_JoystickNumButtons(joysticks[i])
        );
	}

    // DO SHIT
    while (!exit_main_loop) {
    	SDL_JoystickUpdate();
        for (int i = 0; i < joystickCount; i++) {
    		SDL_Joystick *joystick = joysticks[i];
            // int x = SDL_JoystickGetAxis(joystick, 0);
    		// int y = SDL_JoystickGetAxis(joystick, 1);

            uint32_t prev = inputStates[i];
            int buttonCount = SDL_JoystickNumButtons(joystick);
            for (int b = 0; b < buttonCount; b++) {
                inputStates[i] &= ~(1 << b);
                int state = SDL_JoystickGetButton(joystick, b);
                if (state) {
                    inputStates[i] |= (1 << b);
                }
            }

            if (inputStates[i] != prev) {
                for (int p = 31; p >= 0; p--) {
                    buffer[p] = ((inputStates[i] >> p) & 0x1) ? '1' : '0';
                }
                fprintf(stderr, "%d: %32s\n", i, buffer);
            }
        }
    }

	for (int i = 0; i < joystickCount; i++) {
        SDL_JoystickClose(joysticks[i]);
	}
}

int main(int argc, char **argv)
{
	SDL_Init(SDL_INIT_TIMER | SDL_INIT_JOYSTICK);
	SDL_JoystickEventState(SDL_IGNORE);

    signal(SIGINT, sigint_handler);
    main_loop();

	SDL_Quit();
    fprintf(stderr, "done\n");

    return 0;
}
