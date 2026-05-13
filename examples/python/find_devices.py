import mantis
from mantis import errors as me
import argparse

def main():
    parser = argparse.ArgumentParser(description="Find connected devices")
    parser.add_argument("-a", "--args", type=str, default="", help="device args str. Leave empty to find all")
    args = parser.parse_args()

    d_manager = mantis.device_manager.get_instance()

    err, params = mantis.msdr_params.from_str(args.args)
    if err != me.error_code.SUCCESS:
        print(me.mantis_errno(err))
        return

    found_devices = d_manager.find(params)

    if(len(found_devices) == 0):
        print("No devices found")
        return

    for i in range(len(found_devices)):
        print("=================================")
        print(f"Mantis Device {i}")
        print("=================================")
        print(found_devices[i])
        print("\n\n")

if __name__ == "__main__":
    main()