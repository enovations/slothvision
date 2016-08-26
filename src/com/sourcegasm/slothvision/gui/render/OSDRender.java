package com.sourcegasm.slothvision.gui.render;

import com.sourcegasm.slothvision.Launcher;
import com.sourcegasm.slothvision.control.modes.ModeSwitcher;
import com.sourcegasm.slothvision.pimanagement.PiROSData;

import javax.swing.*;
import java.awt.*;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.text.DecimalFormat;

public class OSDRender {

	private static Font font;
	static ImageIcon map = new ImageIcon("./res/map.png");
	static ImageIcon fov = new ImageIcon("./res/fov.png");

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

		if (Launcher.piROSConnector.data.mode == 2) {

			g.setColor(new Color(255, 0, 21, 99));
			g.fillOval(Launcher.piROSConnector.data.markerLoc.x, Launcher.piROSConnector.data.markerLoc.y, 50,50);

		}

		g.setColor(new Color(255, 255, 255, 177));
		g.setFont(g.getFont().deriveFont(22.0f));
		g.drawImage(new ImageIcon("./res/batt_" + ((data.battery < 11.4) ? "off" : "on") + ".png").getImage(), 250, 224,
				46, 18, null);
		g.drawString(new DecimalFormat("#.##").format(data.battery) + "V", 310, 222 + 18);

		g.drawString("M: " + ModeSwitcher.getModeName(Launcher.piROSConnector.data.mode), 470, 222 + 18);

		g.setColor(new Color(241, 246, 255, 116));
		g.drawLine(250, 249, imageW - 250, 249);
		g.drawLine(250, 250, imageW - 250, 250);
		g.drawLine(250, 251, imageW - 250, 251);
		g.drawLine(250, 252, imageW - 250, 252);
		g.drawLine(250, 253, imageW - 250, 253);
		g.drawLine(250, 254, imageW - 250, 254);
		g.drawLine(imageW / 2, 260, imageW / 2, 220);

		g.setColor(new Color(22, 255, 0, 117));
		if (Launcher.hmdSensors.getEulerAngles() != null)
			g.fillRect(imageW / 2 - 9 + (int) Launcher.hmdSensors.getEulerAngles().yaw, 245, 18, 18);

		ByteBuffer buttons = Launcher.joystick.buttonBuffer;

		if (buttons == null || buttons.get(2) == 0) {

			g.setColor(new Color(0, 51, 55, 58));
			g.fillRect(230, imageH - 330, 180, 140);

			g.setColor(new Color(255, 255, 255, 97));

			g.drawLine(250,imageH - 330+69,230+160,imageH - 330+69);
			g.drawLine(250,imageH - 330+70,230+160,imageH - 330+70);
			g.drawLine(250,imageH - 330+71,230+160,imageH - 330+71);

			g.drawLine(230+89, imageH - 330+15, 230+89, imageH - 330+120);
			g.drawLine(230+90, imageH - 330+15, 230+90, imageH - 330+120);
			g.drawLine(230+91, imageH - 330+15, 230+91, imageH - 330+120);

			int yx = (int) (Launcher.joystick.left_lr*-115);
			int yy = (int) (Launcher.joystick.left_ud*-45);

			g.drawRect(230+90-8+yx,imageH - 330+70-8+yy,16,16);
			g.drawRect(230+90-9+yx,imageH - 330+70-9+yy,18,18);
			g.drawRect(230+90-7+yx,imageH - 330+70-7+yy,14,14);

			g.drawLine(230+90,imageH - 330+70,230+90-yx,imageH - 330+70-yy);

			g.drawLine(230+90+1,imageH - 330+70+1,230+90+yx+1,imageH - 330+70+yy+1);
			g.drawLine(230+90,imageH - 330+70,230+90+yx,imageH - 330+70+yy);
			g.drawLine(230+90-1,imageH - 330+70-1,230+90+yx-1,imageH - 330+70+yy-1);

		}

		if (buttons == null || buttons.get(4) == 0) {

			int offsetx = 0;
			int offsety = 0;

			g.clipRect(imageW - 390, imageH - 330, 180, 140);

			g.setColor(new Color(0, 51, 55, 58));
			g.fillRect(imageW - 390, imageH - 330, 180, 140);

			g.setColor(new Color(255, 255, 255, 97));
			g.drawRect(imageW - 390-1, imageH - 330-1, 180+1, 140+1);

			g.rotate(data.yaw, (900 - 390 + 90), (720 - 330 + 70));
			g.drawImage(repos(map.getImage(), offsetx + (int) (data.x * 20), offsety + (int) (data.y * 20)), imageW - 550,
					imageH - 500, null);

			g.rotate(0, (900 - 390 + 75), (720 - 330 + 50));

			if (Launcher.hmdSensors.getEulerAngles() != null)
				g.rotate(Math.toRadians(Launcher.hmdSensors.getEulerAngles().yaw), (imageW - 391 + 90), (imageH - 334 + 70));
			g.drawImage(fov.getImage(), imageW - 391, imageH - 334, null);

		}

		return osd;
	}

	private static BufferedImage repos(Image image, int x, int y) {

		BufferedImage retval = new BufferedImage(image.getWidth(null), image.getHeight(null),
				BufferedImage.TYPE_INT_ARGB);

		Graphics g = retval.getGraphics();

		g.drawImage(image, -x, -y, 800, 800, null);

		return retval;

	}

}