package com.sourcegasm.slothvision.opensv;

import org.bytedeco.javacpp.opencv_core.*;
import org.bytedeco.javacpp.opencv_imgproc.*;
import org.bytedeco.javacv.CanvasFrame;
import org.bytedeco.javacv.Java2DFrameConverter;
import org.bytedeco.javacv.OpenCVFrameConverter;

import javax.swing.*;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;
import java.awt.*;
import java.awt.image.BufferedImage;

import static org.bytedeco.javacpp.opencv_core.*;
import static org.bytedeco.javacpp.opencv_imgproc.*;

public class SlothEyes {

    private int aa, bb, cc, dd, hh, ii;

    private CvScalar blue_min = cvScalar(44, 89, 16, 0);
    private CvScalar blue_max = cvScalar(113, 255, 119, 0);

    private CvScalar red_min = cvScalar(33, 162, 51, 0);
    private CvScalar red_max = cvScalar(53, 255, 200, 0);

    private JSlider sl11 = new JSlider(0, 180, 0);
    private JSlider sl21 = new JSlider(0, 255, 0);
    private JSlider sl31 = new JSlider(0, 255, 0);
    private JSlider sl12 = new JSlider(0, 180, 0);
    private JSlider sl22 = new JSlider(0, 255, 0);
    private JSlider sl32 = new JSlider(0, 255, 0);

    private static CanvasFrame canvas = new CanvasFrame("asdfasdf", 1); //gamma = 1

    public SlothEyes() {
        //for debuging
        //load();
    }

    public LocationData getMarkerData(BufferedImage bufferedimg) {
        if (bufferedimg != null) {

            IplImage sourceRGB = convertBuffToIplImage(bufferedimg);
            //IplImage sourceRGB = AbstractIplImage.createFrom(bufferedimg);
            IplImage source = cvCreateImage(cvGetSize(sourceRGB), 8, 3);

            cvCvtColor(sourceRGB, source, CV_BGR2HSV);

            IplImage thrs_green = hsvThreshold(source, blue_min, blue_max);
            IplImage thrs_red = hsvThreshold(source, red_min, red_max);

            cvErode(thrs_green, thrs_green, null, 15);
            cvErode(thrs_red, thrs_red, null, 15);

            Mat result = new Mat();
            bitwise_or(new Mat(thrs_red), new Mat(thrs_green), result);

//            OpenCVFrameConverter converter = new OpenCVFrameConverter.ToIplImage();
//            canvas.showImage(converter.convert(thrs_red));

            //OpenCVFrameConverter converter = new OpenCVFrameConverter.ToIplImage();
            //canvas.showImage(converter.convert(result));

            CvMoments moments = new CvMoments();

            cvMoments(new IplImage(result), moments, 1);

            double mom10 = cvGetSpatialMoment(moments, 1, 0);
            double mom01 = cvGetSpatialMoment(moments, 0, 1);
            double area = cvGetCentralMoment(moments, 0, 0);

            if (area < 50) {
                return new LocationData(0, 0);
            }

            int posX = (int) (mom10 / area);
            int posY = (int) (mom01 / area);

            LocationData data = new LocationData(posX, posY);
            return data;
        }
        return new LocationData(0, 0);
    }

    private static IplImage hsvThreshold(IplImage orgImg, CvScalar min, CvScalar max) {
        // CvScalar s=cvGet2D(orgImg,120,160);
        // System.out.println( "H:"+ s.val(0) + " S:" + s.val(1) + " V:" +
        // s.val(2) + " X:" + s.val(3));//Print values
        IplImage imgThreshold = cvCreateImage(cvGetSize(orgImg), 8, 1);
        cvInRangeS(orgImg, min, max, imgThreshold);
        // cvReleaseImage(orgImg);
        // cvSmooth(imgThreshold, imgThreshold);
        return imgThreshold;
    }

    public void load() {

        canvas.setDefaultCloseOperation(javax.swing.JFrame.EXIT_ON_CLOSE);

        JFrame d = new JFrame();
        d.setLayout(new GridLayout(6, 2));
        final JLabel l11 = new JLabel("");
        final JLabel l21 = new JLabel("");
        final JLabel l31 = new JLabel("");
        final JLabel l12 = new JLabel("");
        final JLabel l22 = new JLabel("");
        final JLabel l32 = new JLabel("");

        d.add(sl11);
        d.add(l11);
        d.add(sl21);
        d.add(l21);
        d.add(sl31);
        d.add(l31);

        d.add(sl12);
        d.add(l12);
        d.add(sl22);
        d.add(l22);
        d.add(sl32);
        d.add(l32);

        d.setSize(400, 400);
        d.setVisible(true);
        sl11.addChangeListener(new ChangeListener() {
            @Override
            public void stateChanged(ChangeEvent arg0) {
                l11.setText("H min: " + sl11.getValue());
                aa = sl11.getValue();
                red_min = cvScalar(aa, bb, cc, 0);
                red_max = cvScalar(dd, hh, ii, 0);
            }
        });
        sl21.addChangeListener(new ChangeListener() {
            @Override
            public void stateChanged(ChangeEvent arg0) {
                l21.setText("S min: " + sl21.getValue());
                bb = sl21.getValue();
                red_min = cvScalar(aa, bb, cc, 0);
                red_max = cvScalar(dd, hh, ii, 0);
            }
        });
        sl31.addChangeListener(new ChangeListener() {
            @Override
            public void stateChanged(ChangeEvent arg0) {
                l31.setText("V min: " + sl31.getValue());
                cc = sl31.getValue();
                red_min = cvScalar(aa, bb, cc, 0);
                red_max = cvScalar(dd, hh, ii, 0);
            }
        });
        sl12.addChangeListener(new ChangeListener() {
            @Override
            public void stateChanged(ChangeEvent arg0) {
                l12.setText("H max: " + sl12.getValue());
                dd = sl12.getValue();
                red_min = cvScalar(aa, bb, cc, 0);
                red_max = cvScalar(dd, hh, ii, 0);
            }
        });
        sl22.addChangeListener(new ChangeListener() {
            @Override
            public void stateChanged(ChangeEvent arg0) {
                l22.setText("S max: " + sl22.getValue());
                hh = sl22.getValue();
                red_min = cvScalar(aa, bb, cc, 0);
                red_max = cvScalar(dd, hh, ii, 0);
            }
        });
        sl32.addChangeListener(new ChangeListener() {
            @Override
            public void stateChanged(ChangeEvent arg0) {
                l32.setText("V max: " + sl32.getValue());
                ii = sl32.getValue();
                red_min = cvScalar(aa, bb, cc, 0);
                red_max = cvScalar(dd, hh, ii, 0);
            }
        });
    }

    private static IplImage convertBuffToIplImage(BufferedImage original) {
        //convert to bgr
        BufferedImage img = new BufferedImage(original.getWidth(), original.getHeight(), BufferedImage.TYPE_3BYTE_BGR);
        Graphics g = img.getGraphics();
        g.drawImage(original, 0, 0, null);
        Java2DFrameConverter converter1 = new Java2DFrameConverter();
        OpenCVFrameConverter.ToIplImage converter2 = new OpenCVFrameConverter.ToIplImage();
        IplImage iploriginal = converter2.convert(converter1.convert(img));
        return iploriginal;
    }
}
