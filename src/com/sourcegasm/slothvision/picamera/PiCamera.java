package com.sourcegasm.slothvision.picamera;

import java.net.InetAddress;
import java.net.NetworkInterface;
import java.net.SocketException;
import java.util.Enumeration;

public class PiCamera {
    private PiConnectorInstance instance1 = null;
    private PiConnectorInstance instance2 = null;

    public PiCamera(String netInterface) {
        try {
            instance1 = new PiConnectorInstance("192.168.0.50");
        }catch (Exception e){
            System.out.println("COULD NOT CONNECT TO RBPI1 (192.168.0.50)");
        }
        try {
            instance2 = new PiConnectorInstance("192.168.0.51");
        }catch (Exception e){
            System.out.println("COULD NOT CONNECT TO RBPI2 (192.168.0.51)");
        }
        try {
            String ip = getIp(netInterface);
            instance1.setIPAddress(ip);
            instance2.setIPAddress(ip);
            instance1.setPort(5000);
            instance2.setPort(5001);
        }catch (Exception e){
            System.out.println("COUDL NOT FIND IP FOR " + netInterface);
        }
    }

    public void kill(){
        instance1.run();
        instance2.run();
    }

    public void run(){
        instance1.run();
        instance2.run();
    }

    private static String getIp(String ifName) throws SocketException {
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
