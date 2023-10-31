import asyncio
from bleak import BleakScanner, BleakClient, BleakGATTCharacteristic

DEVICE_NAMES = ['BLE1', 'BLE2']

NOTIFY_UUID = '01020304-0506-0708-0900-0a0b0c0d0e0f'
WRITE_UUID = '01020304-0506-0708-0900-0a0b0c0deeff'

cnt = 0

def callback(_: BleakGATTCharacteristic, data: bytearray):
    global cnt
    print(''.join('\\x{:02x}'.format(c) for c in data))

async def main():
    devices = await BleakScanner.discover()
    targets = []
    for device in devices:
        if device.name in DEVICE_NAMES:
            targets.append({'address': device.address, 'name': device.name})
    if len(targets) > 0:
        print('*** DEVICE FOUND')
        print(targets)
        address = targets[0]['address']
        async with BleakClient(address) as client:
            for service in client.services:
                print(f"{service.uuid}: {service.description}: {[f'{c.properties},{c.uuid}' for c in service.characteristics]}")

            await client.start_notify(NOTIFY_UUID, callback)
            await client.write_gatt_char(WRITE_UUID, b'f', response=False)
            await asyncio.sleep(10)
asyncio.run(main())