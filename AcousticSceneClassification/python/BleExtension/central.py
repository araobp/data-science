import asyncio
from bleak import BleakScanner, BleakClient, BleakGATTCharacteristic

import yaml
import sqlite3
import sys

DEVICE_NAMES = ['BLE1', 'BLE2']

NOTIFY_UUID = '01020304-0506-0708-0900-0a0b0c0d0e0f'

cnt = 0

def callback(_: BleakGATTCharacteristic, data: bytearray):
    global cnt
    print(''.join('\\x{:02x}'.format(c) for c in data))

async def central(config):
    devices = await BleakScanner.discover()
    targets = []
    for device in devices:
        if device.name in config['device_names']:
            targets.append({'address': device.address, 'name': device.name})
    if len(targets) > 0:
        print('*** DEVICE FOUND')
        print(targets)
        address = targets[0]['address']
        async with BleakClient(address) as client:
            for service in client.services:
                print(f"{service.uuid}: {service.description}: {[f'{c.properties},{c.uuid}' for c in service.characteristics]}")

            await client.start_notify(config['notify_uuid'], callback)

if __name__ == '__main__':
    with open('config.yml', 'r') as file:
        config = yaml.safe_load(file)

    if config is None:
        sys.exit()
    
    loop = asyncio.get_event_loop()
    try:
        asyncio.ensure_future(central(config))
        loop.run_forever()
    except KeyboardInterrupt:
        pass
    finally:
        print("Closing Loop")
        loop.close()

