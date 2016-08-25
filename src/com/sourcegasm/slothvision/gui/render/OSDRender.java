package com.sourcegasm.slothvision.gui.render;

import com.sourcegasm.slothvision.pimanagement.PiROSData;

import java.awt.*;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;

public class OSDRender {

	private static Font font;

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

			g.setColor(new Color(0, 0, 0, 50));
			g.fillRect(0, imageH - 220, imageW, 40);
			g.setColor(new Color(255, 255, 255, 150));
			g.drawLine(0, imageH - 220 - 1, imageW, imageH - 220 - 1);
			g.drawLine(0, imageH - 220 + 41, imageW, imageH - 220 + 41);
			g.setColor(new Color(255, 255, 255, 255));
			g.setFont(g.getFont().deriveFont(36.0f));
			/*g.drawImage(new ImageIcon("./res/batt_" + ((navData.getBattery() < 25) ? "off" : "on") + ".png").getImage(),
					120, imageH - 218, null);
			g.drawString(navData.getBattery() + "%", 196, imageH - 220 + 34);
			g.drawImage(new ImageIcon("./res/h.png").getImage(), 312, imageH - 218, null);
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