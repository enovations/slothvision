package com.sourcegasm.slothvision.oculus;

public class Main {
    public static void main(String[] args) throws Exception{
        HMDSensors hmdSensors = new HMDSensors(50);
        Thread.sleep(2000);
        while (true){
            Thread.sleep(500);
            System.out.println(hmdSensors.getEulerAngles().yaw);
        }
    }
}
