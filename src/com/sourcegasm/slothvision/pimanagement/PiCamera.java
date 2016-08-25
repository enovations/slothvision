package com.sourcegasm.slothvision.pimanagement;

public class PiCamera {
	private PiConnectorInstance instance1 = null;
	private PiConnectorInstance instance2 = null;

	public PiCamera(String netInterface) {
		try {
			instance1 = new PiConnectorInstance("192.168.0.50");
		} catch (Exception e) {
			System.out.println("COULD NOT CONNECT TO RBPI1 (192.168.0.50)");
		}
		try {
			instance2 = new PiConnectorInstance("192.168.0.51");
		} catch (Exception e) {
			System.out.println("COULD NOT CONNECT TO RBPI2 (192.168.0.51)");
		}
		try {
			String ip = PiConnectorInstance.getIp(netInterface);
			instance1.setIPAddress(ip);
			instance2.setIPAddress(ip);
			instance1.setPort(5000);
			instance2.setPort(5001);
		} catch (Exception e) {
			System.out.println("COUDL NOT FIND IP FOR " + netInterface);
		}
	}

	public void kill() {
		instance1.kill();
		instance2.kill();
	}

	public void run() {
		instance1.run();
		instance2.run();
	}

}
