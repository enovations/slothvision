package com.sourcegasm.slothvision.opensv;

import org.bytedeco.javacpp.opencv_core;
import org.bytedeco.javacpp.opencv_objdetect;
import org.bytedeco.javacv.CanvasFrame;

import static org.bytedeco.javacpp.helper.opencv_objdetect.cvHaarDetectObjects;
import static org.bytedeco.javacpp.opencv_core.*;
import static org.bytedeco.javacpp.opencv_objdetect.CV_HAAR_DO_CANNY_PRUNING;

public class FaceDetection {
    //Load haar classifier XML file
    public static final String XML_FILE =
            "res/haarcascade_frontalface_default.xml";

    private CanvasFrame canvas = new CanvasFrame("asdfasdf", 1); //gamma = 1
    private opencv_objdetect.CvHaarClassifierCascade cascade;

    public FaceDetection(){
        cascade = new opencv_objdetect.CvHaarClassifierCascade(cvLoad(XML_FILE));
    }

    //Detect for face using classifier XML file
    public opencv_core.CvRect detect(opencv_core.IplImage src){

        opencv_core.CvMemStorage storage = opencv_core.CvMemStorage.create();

        //Detect objects
        opencv_core.CvSeq sign = cvHaarDetectObjects(
                src,
                cascade,
                storage,
                1.5,
                3,
                CV_HAAR_DO_CANNY_PRUNING);

        cvClearMemStorage(storage);

        int total_Faces = sign.total();

        if (total_Faces > 0) {
            opencv_core.CvRect r = new CvRect(cvGetSeqElem(sign, 0));
            return r;
        }else {
            return null;
        }

        /*//Draw rectangles around detected objects
        for(int i = 0; i < total_Faces; i++){
            opencv_core.CvRect r = new CvRect(cvGetSeqElem(sign, i));
            cvRectangle (
                    src,
                    cvPoint(r.x(), r.y()),
                    cvPoint(r.width() + r.x(), r.height() + r.y()),
                    opencv_core.CvScalar.RED,
                    2,
                    CV_AA,
                    0);

        }*/

        //Display result
        //cvShowImage("Result", src);
        //OpenCVFrameConverter converter = new OpenCVFrameConverter.ToIplImage();
        //canvas.showImage(converter.convert(src));
    }
}
