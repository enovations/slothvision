package com.sourcegasm.slothvision.gui.render;

import com.sourcegasm.slothvision.Launcher;
import com.sourcegasm.slothvision.pimanagement.PiROSData;

import javax.swing.*;
import java.awt.*;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;
import java.text.DecimalFormat;

public class OSDRender {

	private static Font font;
	static ImageIcon map = new ImageIcon("./res/map.png");

	static {
		try {
			font = Font.createFont(Font.TRUETYPE_FONT, new File("./res/font.ttf"));
		} catch (final FontFormatException | IOException e) {
			e.printStackTrace();
		}
	}

	public static BufferedImage renderOSD(PiROSData data) {
		final int imageW = 900;
		final int imageH = 720;

		final BufferedImage osd = new BufferedImage(imageW, imageH, BufferedImage.TYPE_INT_ARGB);

		final Graphics2D g = (Graphics2D) osd.getGraphics();

		g.setColor(new Color(255, 255, 255, 177));
		g.setFont(g.getFont().deriveFont(22.0f));
		g.drawImage(new ImageIcon("./res/batt_" + ((data.battery < 11.4) ? "off" : "on") + ".png").getImage(), 250, 224,
				46, 18, null);
		g.drawString(new DecimalFormat("#.##").format(data.battery) + "V", 310, 222 + 18);

		g.setColor(new Color(241, 246, 255, 116));
		g.drawLine(250, 249, imageW - 250, 249);
		g.drawLine(250, 250, imageW - 250, 250);
		g.drawLine(250, 251, imageW - 250, 251);
		g.drawLine(250, 252, imageW - 250, 252);
		g.drawLine(250, 253, imageW - 250, 253);
		g.drawLine(250, 254, imageW - 250, 254);
		g.drawLine(imageW / 2, 260, imageW / 2, 220);

		g.setColor(new Color(22, 255, 0, 117));
		g.fillRect(imageW / 2 - 9 + (int) Launcher.hmdSensors.getEulerAngles().yaw, 245, 18, 18);

		/*g.setColor(new Color(0, 0, 0, 141));
		g.fillRect(200, 220, 120, 90);

		g.setColor(new Color(219, 3, 0, 141));
		for (PiROSData.Point point : data.points) {
			g.fillRect(260 - 2 + (int) (point.x * 10), 265 - 2 + (int) (point.y * 10), 4, 4);
		}*/

		g.setColor(new Color(0, 0, 0, 49));
		g.fillRect(imageW - 350, imageH - 300, 130, 100);

		//2.5cm je 1 pixel

		//g.rotate(0.0);
		g.clipRect(imageW - 350, imageH - 300, 130, 100);

		g.drawImage(map.getImage(), imageW - 350, imageH - 300, null);

		//after
		g.setClip(null);

			/*g.drawImage(new ImageIcon("./res/h.png").getImage(), 312, imageH - 218, null);
			g.drawString(((int) (navData.getAltitude() * 100.0f)) + "cm", 358, imageH - 220 + 34);
			g.drawImage(new ImageIcon("./res/" + ((navData.isFlying()) ? "on" : "off") + ".png").getImage(), 490,
					imageH - 218, null);
			g.drawImage(new ImageIcon("./res/roll.png").getImage(), 560, imageH - 218, null);
			g.drawString((int) (navData.getRoll()) + "° # " + ((int) navData.getPitch()) + "°", 610, imageH - 220 + 34);

			g.setColor(new Color(255, 255, 255, 255));
			g.drawString(mainController.controlMode.toString(), 170, 200);

			g.setFont(g.getFont().deriveFont(42.0f));

			if (!navData.isFlying() && !timer.show)
				g.drawString("Press L2 button to take off", 205, 370);
			else if (timer.show) {
				g.setFont(font.deriveFont(68.0f));
				g.drawString(String.format("%.2f", timer.timer), 400, 300);
			}

			g.setColor(new Color(0, 0, 0, 90));
			g.fillRect(imageW - 320 + 50, 130, imageW - (imageW - 320 + 50), 100);
			g.rotate(Math.toRadians(navData.getYaw()), imageW - 320 + 50, 130 + 50);
			g.drawImage(new ImageIcon("./res/rotate.png").getImage(), imageW - 320, 130, null);*/

		return osd;
	}

}