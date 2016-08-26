package com.sourcegasm.slothvision.pimanagement;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;

public class PiROSConnector implements Runnable {

	private PiConnectorInstance instance;
	private boolean running = true;

	public PiROSData data = new PiROSData();

	public PiROSConnector(String interfacel) {
		//send: ip: 192.168.0.100
		try {
			instance = new PiConnectorInstance("192.168.0.100", 8008);
		} catch (Exception e) {
			System.out.println("COULD NOT CONNECT TO RBPI1 (192.168.0.50)");
		}
		try {
			String ip = PiConnectorInstance.getIp(interfacel);
			instance.setIPAddress(ip);
			instance.setPort(5002);
		} catch (Exception e) {
			System.out.println("COUDL NOT FIND IP FOR " + interfacel);
		}

		Thread t = new Thread(this);
		t.start();

	}

	public void kill() {
		instance.kill();
		running = false;
	}

	public void runConnector() {
		instance.run();
	}

	@Override public void run() {

		try {
			DatagramSocket serverSocket = new DatagramSocket(5002);
			byte[] receiveData = new byte[256];

			DatagramPacket receivePacket = new DatagramPacket(receiveData, receiveData.length);

			while (running) {
				serverSocket.receive(receivePacket);
				String sentence = new String(receivePacket.getData(), 0, receivePacket.getLength());

				if (sentence.startsWith("battery: ")) {
					data.battery = Float.parseFloat(sentence.replace("battery: ", "").trim());
				} else if (sentence.startsWith("sonar new")) {
					data.points.clear();
				} else if (sentence.startsWith("sonar: ")) {
					data.points
							.add(new PiROSData.Point(Double.parseDouble(sentence.replace("sonar: ", "").split(" ")[0]),
									Double.parseDouble(sentence.replace("sonar: ", "").split(" ")[1])));
				} else if (sentence.startsWith("position: ")) {
					data.x = Double.parseDouble(sentence.replace("position: ", "").split(" ")[0]);
					data.y = Double.parseDouble(sentence.replace("position: ", "").split(" ")[1]);
					data.yaw = Double.parseDouble(sentence.replace("position: ", "").split(" ")[2]);
				} else {
					System.out.println("Unknown message from ros pi: " + sentence);
				}
			}
		} catch (IOException e) {
			System.out.println("Connection error! -42");
		}

	}
}
