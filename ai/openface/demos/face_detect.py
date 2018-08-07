import argparse
import cv2
import os
import dlib

import numpy as np
np.set_printoptions(precision=2)
import openface

from matplotlib import cm

fileDir = os.path.dirname(os.path.realpath(__file__))
modelDir = os.path.join(fileDir, '..', 'models')
dlibModelDir = os.path.join(modelDir, 'dlib')

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument(
        '--dlibFacePredictor',
        type=str,
        help="Path to dlib's face predictor.",
        default=os.path.join(
            dlibModelDir,
            "shape_predictor_68_face_landmarks.dat"))
    parser.add_argument(
        '--networkModel',
        type=str,
        help="Path to Torch network model.",
        default='models/openface/nn4.v1.t7')
    # Download model from:
    # https://storage.cmusatyalab.org/openface-models/nn4.v1.t7
    parser.add_argument('--imgDim', type=int,
                        help="Default image dimension.", default=96)
    # parser.add_argument('--width', type=int, default=640)
    # parser.add_argument('--height', type=int, default=480)
    parser.add_argument('--width', type=int, default=1280)
    parser.add_argument('--height', type=int, default=800)
    parser.add_argument('--scale', type=int, default=1.0)
    parser.add_argument('--cuda', action='store_true')
    parser.add_argument('--image', type=str,help='Path of image to recognition')

    args = parser.parse_args()
    if (None == args.image) or (not os.path.exists(args.image)):
        print '--image not set or image file not exists'
        exit()

    align = openface.AlignDlib(args.dlibFacePredictor)
    net = openface.TorchNeuralNet(
        args.networkModel,
        imgDim=args.imgDim,
        cuda=args.cuda)

    cv2.namedWindow('video', cv2.WINDOW_NORMAL)

    frame = cv2.imread(args.image)  
    bbs = align.getAllFaceBoundingBoxes(frame)
    for i, bb in enumerate(bbs):
    # landmarkIndices set  "https://cmusatyalab.github.io/openface/models-and-accuracies/"
        alignedFace = align.align(96, frame, bb,
                                      landmarkIndices=openface.AlignDlib.OUTER_EYES_AND_NOSE)
        rep = net.forward(alignedFace)

        center = bb.center()
        centerI = 0.7 * center.x * center.y / \
                (args.scale * args.scale * args.width * args.height)
        color_np = cm.Set1(centerI)
        color_cv = list(np.multiply(color_np[:3], 255))

        bl = (int(bb.left() / args.scale), int(bb.bottom() / args.scale))
        tr = (int(bb.right() / args.scale), int(bb.top() / args.scale))
        cv2.rectangle(frame, bl, tr, color=color_cv, thickness=3)

    cv2.imshow('video', frame)

    cv2.waitKey (0)  

    cv2.destroyAllWindows()
