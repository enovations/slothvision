package com.sourcegasm.slothvision.opengl;

import static org.lwjgl.glfw.GLFW.*;
import static org.lwjgl.opengl.GL11.*;
import static org.lwjgl.opengl.GL20.glDisableVertexAttribArray;
import static org.lwjgl.opengl.GL20.glEnableVertexAttribArray;
import static org.lwjgl.opengl.GL20.glUseProgram;
import static org.lwjgl.opengl.GL30.glBindVertexArray;
import static org.lwjgl.system.MemoryUtil.NULL;

import java.awt.image.BufferedImage;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.nio.FloatBuffer;

import org.lwjgl.glfw.GLFWKeyCallback;
import org.lwjgl.opengl.GL;

public class OpenGLWindow {

	public boolean running;

	private long window;

	private final int width = 1280, height = 800;

	public GLFWKeyCallback keyCallback = null;

	public Model leftSide, rightSide;

	public BufferedImage rightImage;
	public BufferedImage leftImage;
	int lid, rid;

    public String host = "192.168.0.100";
    public int port = 12345;

    public DatagramSocket sock;
    public InetAddress IPAddress;

    public OpenGLWindow() {
	    try {
            sock = new DatagramSocket();
            IPAddress = InetAddress.getByName(host);
        } catch (Exception e) {
            e.printStackTrace();
        }

		keyCallback = new GLFWKeyCallback() {
			@Override public void invoke(long arg0, int arg1, int arg2, int arg3, int arg4) {
				if (arg1 == GLFW_KEY_ESCAPE && arg3 == GLFW_PRESS) {
					running = false;
				}
			}
		};
	}

	public void start() {
		running = true;
		init();

		long startTime = System.currentTimeMillis();
		long delta = 0;
		final float interval = 1000f / 30f;

		while (running) {
			final long now = System.currentTimeMillis();
			delta += now - startTime;
			startTime = now;

			if (leftImage != null)
				TextureLoader.loadTexture(leftImage,lid);

			if (rightImage != null)
				TextureLoader.loadTexture(rightImage,rid);

			while (delta >= interval) {
				update();
				delta -= interval;
			}
			render();

			if (glfwWindowShouldClose(window)) {
				running = false;
			}
		}
	}

	public void init() {
		if (!glfwInit()) {
			System.err.println("GLFW initialization failed");
		}

		glfwDefaultWindowHints();
		glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		window = glfwCreateWindow(width, height, "SlothVision", glfwGetMonitors().get(2), NULL);
		//window = glfwCreateWindow(width, height, "SlothVision", glfwGetPrimaryMonitor(), NULL);
		//window = glfwCreateWindow(width, height, "SlothVision", NULL, NULL);

		if (window == NULL) {
			System.err.println("Could not create our Window!");
		}

		glfwMakeContextCurrent(window);
		GL.createCapabilities();
		glfwSwapInterval(1);
		//glfwShowWindow(window);

		glClearColor(0f, 0f, 0f, 1f);

		glfwSetKeyCallback(window, keyCallback);

		BufferedImage texture = TextureLoader.loadImage("res/grid.png");

		leftImage = texture;
		rightImage = texture;

		lid = glGenTextures();
		rid = glGenTextures();

		final int[] indices = MeshMaker.indices();
		final float[] textureCoords = MeshMaker.textureCoords();

		leftSide = new Model(MeshMaker.leftMesh(), indices, textureCoords, texture, "shaders/vertexShaderLeft.txt",
				"shaders/fragmentShader.txt", lid);
		rightSide = new Model(MeshMaker.rightMesh(), indices, textureCoords, texture, "shaders/vertexShaderRight.txt",
				"shaders/fragmentShader.txt", rid);
	}

	public void update() {
	    // desno levo desno gor desno desno desno gor

        float maxRobotSpeed = 1.0f;
        float maxGimbalo = 500;

        glfwPollEvents();

        FloatBuffer buffer = glfwGetJoystickAxes(GLFW_JOYSTICK_1);

        float robotLeftRight = buffer.get() * -1 * maxRobotSpeed;
        if (robotLeftRight < 0.01 && robotLeftRight > -0.01) {
            robotLeftRight = 0;
        }
        float robotUpDown = buffer.get() * -1 * maxRobotSpeed;
        if (robotUpDown < 0.01 && robotUpDown > -0.01) {
            robotUpDown = 0;
        }

        float gimbaloLeftRight = buffer.get() * maxGimbalo;
        float gimbaloUpDown = buffer.get() * maxGimbalo;

        try {
            String robotMessageString = "r " + robotUpDown + " " + robotLeftRight;

            byte[] sendData = robotMessageString.getBytes();
            DatagramPacket sendPacket = new DatagramPacket(sendData, sendData.length, IPAddress, port);
            sock.send(sendPacket);

            String gimbaloMessageString = "g " + gimbaloLeftRight + " " + gimbaloUpDown;
            sendData = gimbaloMessageString.getBytes();
            sendPacket = new DatagramPacket(sendData, sendData.length, IPAddress, port);
            sock.send(sendPacket);
        } catch (Exception e) {
            e.printStackTrace();
        }

        System.out.println(gimbaloLeftRight + " " + gimbaloUpDown);
    }

	public void render() {
		glfwSwapBuffers(window);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Left side
		glUseProgram(leftSide.programID);

		glBindVertexArray(leftSide.vao);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glBindTexture(GL_TEXTURE_2D, lid);
		glDrawElements(GL_TRIANGLES, leftSide.size, GL_UNSIGNED_INT, 0);
		//glBindTexture(GL_TEXTURE_2D, 0);
		//glDeleteTextures(leftSide.textureID);
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glBindVertexArray(0);

		glUseProgram(0);

		// Right side
		glUseProgram(rightSide.programID);

		glBindVertexArray(rightSide.vao);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glBindTexture(GL_TEXTURE_2D, rid);
		glDrawElements(GL_TRIANGLES, rightSide.size, GL_UNSIGNED_INT, 0);
		//glBindTexture(GL_TEXTURE_2D, 0);
		//glDeleteTextures(rightSide.textureID);
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glBindVertexArray(0);

		glUseProgram(0);
	}
}