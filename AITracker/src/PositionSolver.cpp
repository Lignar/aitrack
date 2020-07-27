#include "PositionSolver.h"


PositionSolver::PositionSolver(int width, int height, 
    float prior_pitch, float prior_yaw, float prior_distance):
    contour_indices{ 0,1,8,15,16,27,28,29,30,31,32,33,34,35,36,39,42,45 }
{
    this->prior_pitch = (1.1f * (prior_pitch + 90.f) / 180.f) - (double)2.5f;
    this->prior_distance = prior_distance * -2. ;
    this->prior_yaw = (1.84f*(prior_yaw + 90.f)/180.f) - (double)3.14f;

    std::cout << "PRIORS CALCULATED: \nPITCH: " <<this->prior_pitch << "  YAW: " << this->prior_yaw << "  DISTANCE: " << this->prior_distance;

    mat3dcontour = (cv::Mat_<double>(18, 3) <<
        0.45517698, 0.30089578, -0.76442945,
        0.44899884, 0.16699584, -0.765143,
        0., -0.621079, -0.28729478,
        -0.44899884, 0.16699584, -0.765143,
        -0.45517698, 0.30089578, -0.76442945,
        0., 0.2933326, -0.1375821,
        0., 0.1948287, -0.06915811,
        0., 0.10384402, -0.00915182,
        0., 0., 0.,
        0.08062635, -0.04127607, -0.13416104,
        0.04643935, -0.05767522, -0.10299063,
        0., -0.06875312, -0.09054535,
        -0.04643935, -0.05767522, -0.10299063,
        -0.08062635, -0.04127607, -0.13416104,
        0.31590518, 0.2983375, -0.2851074,
        0.13122973, 0.28444737, -0.23423915,
        -0.13122973, 0.28444737, -0.23423915,
        -0.31590518, 0.2983375, -0.2851074
      );

    mat3dface = (cv::Mat_<double>(70, 3) <<
        0.4551769692672, 0.300895790030204, -0.764429433974752,
        0.448998827123556, 0.166995837790733, -0.765143004071253,
        0.437431554952677, 0.022655479179981, -0.739267175112735,
        0.415033422928434, -0.088941454648772, -0.747947437846473,
        0.389123587370091, -0.232380029794684, -0.704788385327458,
        0.334630113904382, -0.361265387599081, -0.615587579236862,
        0.263725112132858, -0.460009725616771, -0.491479221041573,
        0.16241621322721, -0.558037146073869, -0.339445180872282,
        0., -0.621079019321682, -0.287294770748887,
        -0.16241621322721, -0.558037146073869, -0.339445180872282,
        -0.263725112132858, -0.460009725616771, -0.491479221041573,
        -0.334630113904382, -0.361265387599081, -0.615587579236862,
        -0.389123587370091, -0.232380029794684, -0.704788385327458,
        -0.415033422928434, -0.088941454648772, -0.747947437846473,
        -0.437431554952677, 0.022655479179981, -0.739267175112735,
        -0.448998827123556, 0.166995837790733, -0.765143004071253,
        -0.4551769692672, 0.300895790030204, -0.764429433974752,
        0.385529968662985, 0.402800553948697, -0.310031082540741,
        0.322196658344302, 0.464439136821772, -0.250558059367669,
        0.25409760441282, 0.46420381416882, -0.208177722146526,
        0.186875436782135, 0.44706071961879, -0.145299823706503,
        0.120880983543622, 0.423566314072968, -0.110757158774771,
        -0.120880983543622, 0.423566314072968, -0.110757158774771,
        -0.186875436782135, 0.44706071961879, -0.145299823706503,
        -0.25409760441282, 0.46420381416882, -0.208177722146526,
        -0.322196658344302, 0.464439136821772, -0.250558059367669,
        -0.385529968662985, 0.402800553948697, -0.310031082540741,
        0., 0.293332603215811, -0.137582088779393,
        0., 0.194828701837823, -0.069158109325951,
        0., 0.103844017393155, -0.009151819844964,
        0., 0., 0.,
        0.080626352317973, -0.041276068128093, -0.134161035564826,
        0.046439347377934, -0.057675223874769, -0.102990627164664,
        0., -0.068753126205604, -0.090545348482397,
        -0.046439347377934, -0.057675223874769, -0.102990627164664,
        -0.080626352317973, -0.041276068128093, -0.134161035564826,
        0.315905195966084, 0.298337502555443, -0.285107407636464,
        0.275252345439353, 0.312721904921771, -0.244558251170671,
        0.176394511553111, 0.311907184376107, -0.219205360345231,
        0.131229723798772, 0.284447361805627, -0.234239149487417,
        0.184124948330084, 0.260179585304867, -0.226590776513707,
        0.279433549294448, 0.267363071770222, -0.248441437111633,
        -0.131229723798772, 0.284447361805627, -0.234239149487417,
        -0.176394511553111, 0.311907184376107, -0.219205360345231,
        -0.275252345439353, 0.312721904921771, -0.244558251170671,
        -0.315905195966084, 0.298337502555443, -0.285107407636464,
        -0.279433549294448, 0.267363071770222, -0.248441437111633,
        -0.184124948330084, 0.260179585304867, -0.226590776513707,
        0.121155252430729, -0.208988660580347, -0.160606287940521,
        0.041356305910044, -0.194484199722098, -0.096159882202821,
        0., -0.205180167345702, -0.083299217789729,
        -0.041356305910044, -0.194484199722098, -0.096159882202821,
        -0.121155252430729, -0.208988660580347, -0.160606287940521,
        -0.132325402795928, -0.290857984604968, -0.187067868218105,
        -0.064137791831655, -0.325377847425684, -0.158924039726607,
        0., -0.343742581679188, -0.113925986025684,
        0.064137791831655, -0.325377847425684, -0.158924039726607,
        0.132325402795928, -0.290857984604968, -0.187067868218105,
        0.181481567104525, -0.243239316141725, -0.231284988892766,
        0.083999507750469, -0.239717753728704, -0.155256465640701,
        0., -0.256058040176369, -0.0950619498899,
        -0.083999507750469, -0.239717753728704, -0.155256465640701,
        -0.181481567104525, -0.243239316141725, -0.231284988892766,
        -0.074036069749345, -0.250689938345682, -0.177346470406188,
        0., -0.264945854681568, -0.112349967428413,
        0.074036069749345, -0.250689938345682, -0.177346470406188,
        0.257990002632141, 0.276080012321472, -0.219998998939991,
        -0.257990002632141, 0.276080012321472, -0.219998998939991,
        0.257990002632141, 0.276080012321472, -0.324570998549461,
        -0.257990002632141, 0.276080012321472, -0.324570998549461
        );

    camera_matrix = (cv::Mat_<double>(3, 3) <<
        height, 0, height / 2,
        0, height, width / 2,
        0, 0, 1
    );

    camera_distortion = (cv::Mat_<double>(4, 1) << 0, 0, 0, 0);
}

void PositionSolver::solve_rotation(FaceData* face_data)
{
    std::vector<cv::Point2f> landmarkPoints;

    for (int i : this->contour_indices)
    {
        landmarkPoints.push_back(
            cv::Point(face_data->landmark_coords[2 * i], face_data->landmark_coords[2 * i + 1])
        );
    }

    std::vector<double> rv({ prior_pitch, prior_yaw, 0 }), tv({0, 0, prior_distance});
    cv::Mat rvec(rv), tvec(tv);

    cv::Mat ip(landmarkPoints);

    solvePnP(mat3dcontour,
        ip, 
        this->camera_matrix, 
        this->camera_distortion, 
        rvec, 
        tvec, 
        true, //extrinsic guess
        cv::SOLVEPNP_ITERATIVE
        );

    //std::cout << rvec << std::endl;

    cv::Mat rotM(3, 3, CV_64FC1);
    cv::Rodrigues(rvec, rotM);


    cv::Mat concated(3, 4, CV_64FC1);
    cv::hconcat(rotM, tvec, concated);


    cv::decomposeProjectionMatrix(
        concated, 
        cv::Mat(3, 3, CV_64FC1), 
        cv::Mat(3, 3, CV_64FC1), 
        cv::Mat(4, 1, CV_64FC1), 
        cv::Mat(3, 3, CV_64FC1), 
        cv::Mat(3, 3, CV_64FC1), 
        cv::Mat(3, 3, CV_64FC1), 
        rvec
    );


    //double d = 6;

    //Correct relative translation
    //double yaw_angle = rvec.at<double>(1,0);
    //double y_trans = tvec.at<double>(1, 0);  //Bien
    //double correction = y_trans <= 0 ? yaw_angle + std::atan(y_trans / d) : yaw_angle - std::atan(y_trans / d);


    for (int i = 0; i < 3; i++)
    {
        face_data->rotation[i] = rvec.at<double>(i, 0);
        face_data->translation[i] = tvec.at<double>(i, 0);
    }

    //face_data->rotation[1] = correction;

    //std::cout << "FACEDATA\n" << face_data->to_string()<<std::endl; 
 }


