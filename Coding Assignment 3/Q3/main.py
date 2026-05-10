import platform

from host_info import WindowsHost
from host_info import LinuxHost


def main():

    os_name = platform.system()

    if os_name == "Windows":

        host = WindowsHost()

    elif os_name == "Linux":

        host = LinuxHost()

    else:

        raise Exception(
            "Unsupported Operating System")

    host.get_hardware_info()

    host.display_hardware_info()


if __name__ == "__main__":
    main()