package com.sourcegasm.slothvision.control;

import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;

public class ROSControl {

	private String host = "192.168.0.100";
	private int port = 12345;

	private DatagramSocket sock;
	private InetAddress IPAddress;

	public double speed, steer, pan, tilt;

	public ROSControl(){
		try {
			sock = new DatagramSocket();
			IPAddress = InetAddress.getByName(host);
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	public void sendUpdate(){
		try {
			String robotMessageString = "r " + speed + " " + steer;

			byte[] sendData = robotMessageString.getBytes();
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
