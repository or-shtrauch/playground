use core::net;
use std::net::Ipv4Addr;

#[derive(Debug, Clone)]
struct Device {
    id: u32,
    name: String,
    online: bool,
    ipv4: Option<net::Ipv4Addr>,
}

type DeviceLeft = Option<fn(&mut Device)>;
type DeviceJoined = Option<fn(&mut Device)>;
type DeviceChangedIPV4 = Option<fn(&mut Device, &net::Ipv4Addr)>;

#[derive(Debug, Clone)]
enum NetworkEvent {
    DeviceLeft(DeviceLeft),
    DeviceJoined(DeviceJoined),
    DeviceChangedIPV4(DeviceChangedIPV4),
}

struct NetworkPlugin {
    device_left: DeviceLeft,
    device_joined: DeviceJoined,
    device_changed_ipv4: DeviceChangedIPV4,
}

struct NetworkMap {
    devices: Vec<Box<Device>>,
    plugins: Vec<NetworkPlugin>,
}

impl Default for NetworkPlugin {
    fn default() -> Self {
        NetworkPlugin {
            device_left: None,
            device_joined: None,
            device_changed_ipv4: None,
        }
    }
}

impl NetworkPlugin {
    fn new(events: Vec<NetworkEvent>) -> Self {
        let mut plugin = NetworkPlugin::default();
        plugin.add_events(events);

        plugin
    }

    fn add_events(&mut self, events: Vec<NetworkEvent>) {
        for event in events {
            match event {
                NetworkEvent::DeviceLeft(cb) => self.device_left = cb,
                NetworkEvent::DeviceJoined(cb) => self.device_joined = cb,
                NetworkEvent::DeviceChangedIPV4(cb) => self.device_changed_ipv4 = cb,
            }
        }
    }
}

impl NetworkMap {
    fn add_device(&mut self, device: Device) {
        let mut device = Box::from(device);

        for plug in self.plugins.iter_mut() {
            if let Some(cb) = plug.device_joined {
                cb(&mut device);
            }
        }

        self.devices.push(device);
    }

    fn remove_device(&mut self, device: Device) {
        let mut index = None;

        for (i, dev) in self.devices.iter_mut().enumerate() {
            if dev.id == device.id {
                index = Some(i);
                break;
            }
        }

        if let Some(i) = index {
            let mut dev = self.devices.swap_remove(i);

            for plug in self.plugins.iter_mut() {
                if let Some(cb) = plug.device_left {
                    cb(&mut dev);
                }
            }

            dev.online = false;
        }
    }

    fn change_device_ipv4(&mut self, device: Device, ipv4: &net::Ipv4Addr) {
        for dev in self.devices.iter_mut() {
            if dev.id == device.id {
                dev.online = true;
                for plug in self.plugins.iter_mut() {
                    if let Some(cb) = plug.device_changed_ipv4 {
                        cb(dev, &ipv4);
                    }
                }

                dev.ipv4 = Some(net::Ipv4Addr::clone(ipv4));
            }
        }
    }
}

fn device_left(device: &mut Device) {
    println!("Device left: {}", device.name);
}

fn device_joined(device: &mut Device) {
    println!("Device joined: {}", device.name);
}

fn device_changed_ipv4(device: &mut Device, ipv4: &net::Ipv4Addr) {
    let old_ipv4 = device.ipv4.unwrap_or(Ipv4Addr::new(0, 0, 0, 0));
    println!(
        "Device changed ipv4: {} - {}-> {}",
        device.name, old_ipv4, ipv4
    );
}

fn main() {
    let plug1 = NetworkPlugin::new(vec![
        NetworkEvent::DeviceLeft(Some(device_left)),
        NetworkEvent::DeviceChangedIPV4(Some(device_changed_ipv4)),
    ]);
    let plug2 = NetworkPlugin::new(vec![
        NetworkEvent::DeviceJoined(Some(device_joined))
    ]);
    let plug3 = NetworkPlugin::new(vec![
        NetworkEvent::DeviceLeft(Some(device_left)),
        NetworkEvent::DeviceJoined(Some(device_joined)),
    ]);

    let mut network_map = NetworkMap {
        devices: vec![],
        plugins: vec![plug1, plug2, plug3],
    };

    network_map.add_device(Device {
        id: 1,
        name: String::from("device1"),
        online: true,
        ipv4: None,
    });
    network_map.add_device(Device {
        id: 2,
        name: String::from("device2"),
        online: true,
        ipv4: None,
    });

    network_map.change_device_ipv4(
        Device {
            id: 1,
            name: String::from("device1"),
            online: true,
            ipv4: None,
        },
        &net::Ipv4Addr::new(192, 168, 1, 1),
    );

    network_map.remove_device(Device {
        id: 1,
        name: String::from("device1"),
        online: true,
        ipv4: None,
    });
}
