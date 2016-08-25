package com.sourcegasm.slothvision.pimanagement;

import java.io.IOException;
import java.net.*;
import java.util.Enumeration;

public class PiConnectorInstance {
	private String host = "192.168.0.100";
	private int port = 8008;

	private DatagramSocket sock;
	private InetAddress IPAddress;

	public PiConnectorInstance(String host) throws Exception {
		this.host = host;
		connect();
	}

	public PiConnectorInstance(String host, int port) throws Exception {
		this.host = host;
		this.port = port;
		connect();
	}

	private void connect() throws Exception {
		sock = new DatagramSocket();
		IPAddress = InetAddress.getByName(host);
	}

	private void sendMessage(String msg) {
		try {
			msg += "\n";
			byte[] sendData = msg.getBytes();
			DatagramPacket sendPacket = new DatagramPacket(sendData, sendData.length, IPAddress, port);
			sock.send(sendPacket);
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

	public void setIPAddress(String add) {
		sendMessage("ip: " + add);
	}

	public void setPort(int port) {
		sendMessage("port: " + port);
	}

	public void run() {
		kill();
		sendMessage("run");
	}

	public void run(int port) {
		kill();
		setPort(port);
		sendMessage("run");
	}

	public void kill() {
		sendMessage("kill");
	}

	public static String getIp(String ifName) throws SocketException {
		for (Enumeration<NetworkInterface> en = NetworkInterface.getNetworkInterfaces(); en.hasMoreElements(); ) {
			NetworkInterface intf = en.nextElement();
			if (intf.getName().equals(ifName)) {
				for (Enumeration<InetAddress> enumIpAddr = intf.getInetAddresses(); enumIpAddr.hasMoreElements(); ) {
					String asdf = enumIpAddr.nextElement().toString();
					if (asdf.contains(".")) {
						return asdf.substring(1);
					}
				}
			}
		}
		throw new SocketException();
	}

}
