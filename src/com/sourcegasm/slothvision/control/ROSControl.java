package com.sourcegasm.slothvision.control;

import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;

class ROSControl {

	private DatagramSocket sock;
	private InetAddress IPAddress;

	double speed, steer, pan, tilt;

	ROSControl() {
		try {
			sock = new DatagramSocket();
			String host = "192.168.0.100";
			IPAddress = InetAddress.getByName(host);
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	void sendUpdate() {
		try {
			String robotMessageString = "r " + speed + " " + steer;

			byte[] sendData = robotMessageString.getBytes();
			int port = 8008;
			DatagramPacket sendPacket = new DatagramPacket(sendData, sendData.length, IPAddress, port);
			sock.send(sendPacket);

			String gimbaloMessageString = "g " + pan + " " + tilt;
			sendData = gimbaloMessageString.getBytes();
			sendPacket = new DatagramPacket(sendData, sendData.length, IPAddress, port);
			sock.send(sendPacket);

		} catch (Exception e) {
			e.printStackTrace();
		}
	}

}
