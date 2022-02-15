
//////////////////////////////////////////////////////////////////////////////////////////////////////////
// Projet de Traitement des Images et des Vidéos
// Option DATA
// Equipe : Nour Zbitou, Rodrigue Bourel, Melissande Mélissande, Arthur Moulinier
//////////////////////////////////////////////////////////////////////////////////////////////////////////

// problème sur la 01316

// CPP program to demonstrate associative arrays
#include <bits/stdc++.h>
#include <iostream>
using namespace std;

#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include <sys/stat.h>
using namespace cv;

#include "histogram.hpp"

//Definit le nombre de dossier et fichier
#define NB_CANDIDATS 35
#define NB_SAMPLES 22

#define ROWS 7
#define COLS 5

#define CROSSREGION_H 1200

#define WIDTH 2480
#define HEIGHT 3508

#define TOPCROSS_X 2140
#define TOPCROSS_Y 409
#define BOTCROSS_X 197
#define BOTCROSS_Y 3175

#define ICON_X 200
#define ICON_Y 680
#define ICON_W 300
#define ICON_H 350

#define IMAGETTE_T 245
#define IMAGETTE_X 610
#define IMAGETTE_Y 725

Point ab = {TOPCROSS_X - BOTCROSS_X, TOPCROSS_Y - BOTCROSS_Y};

string repoSource = "../../../NicIcon/all-scans/";
string repoDest = "../../../preProcessedIcon/";

map<string, double> icons = {
        {"accident", 20000000},
        {"person", 20000000},
        {"electricity", 20000000},
        {"injury", 20000000},
        {"bomb", 30000000},
        {"gas", 30000000},
        {"flood", 30000000},
        {"casualty", 40000000},
        {"fireBrigade", 40000000},
        {"car", 40000000},
        {"fire", 40000000},
        {"police", 40000000},
        {"roadBlock", 50000000},
        {"paramedics", 60000000}
};

map<string, double> sizeIcons = {
        {"large", 20000000},
        {"medium", 20000000},
        {"small", 20000000},
};

struct DetectionStruct {
    bool detected = false;
    Point loc = Point(-1,-1);
    Point templ = Point(-1,-1);
    string type = "none";
};

/**
 * crée les repertoires necessaire à l'ecriture des fichier txt et png
 */
void createDir() {
    int x;
    cout << "Can I create the directory you need, type 1 if yes, something else otherwise : \n"; // Type a number and press enter
    cin >> x; // Get user input from the keyboard
    if(x == 1) {
        mkdir("../../../preProcessedIcon/Imagettes", 0777);
        mkdir("../../../preProcessedIcon/Final", 0777);
    }
}

/**
 *
 * @param imageName le nom de l'image (le nom du fichier .png)
 * @return une matrice de pixels qui est le png upload
 */
Mat uploadImage (string imageName){
    Mat im = imread(imageName);
    if(im.data == nullptr){
        cerr << "Image not found: "<< imageName << endl;
        waitKey(0);
        //system("pause");
        exit(EXIT_FAILURE);
    }
    return im;
}

/**
 *
 * @param image notre image
 * @param scale la mise à l'echelle à appliquée à l'image
 * @return notre image avec une mise a l'echelle scale
 */
Mat augmente (Mat image, double scale) {
    double w = image.cols*scale;
    double h = image.rows*scale;
    int width = (int) w;
    int height = (int) h;
    Size taille(width, height);
    Mat reshape = Mat(taille,CV_8UC3); //cree une image à 3 canaux de profondeur 8 bits chacuns

    resize(image,reshape,taille);
    return reshape;
}

/**
 *
 * @param value un int
 * @return un string qui est la conversion du int en string de 2 caractères
 */
string intTo2digits (int value){
    if (value < 0 || value >= 100){
        cerr << "intTo2digit out of bounds value : " << value << endl;
        exit(EXIT_FAILURE);
    }
    if(value < 10)
        return "0" + to_string(value);
    return to_string(value);
}

/**
 *
 * @param value un int
 * @return un string qui est la conversion du int en string de 3 caractères
 */
string intTo3digits (int value){
    if (value < 0 || value >= 1000){
        cerr << "intTo3digit out of bounds value : " << value << endl;
        exit(EXIT_FAILURE);
    }
    if(value < 10)
        return "00" + to_string(value);
    if(value < 100)
        return "0" + to_string(value);
    return to_string(value);
}

/**
 *
 * @param image notre image
 * @param zone la zone de detection
 * @param templName le nom du template pour faire un matchTemplate entre nos icones dans l'image et le template en dur
 * @param treshold
 * @return un detectionStruct contenant les informations (position,type,...) du texte detecté dans notre zone de detection
 */
DetectionStruct templateDetection(Mat image, Rect zone, string templName, int treshold){
    Mat imZone = image(zone);
    Mat templ = uploadImage("../src/Template/" + templName + ".png");
    Mat match;
    DetectionStruct res;

    matchTemplate(imZone, templ, match,  TM_SQDIFF);
    double minVal; double maxVal; Point minLoc; Point maxLoc;
    minMaxLoc( match, &minVal, &maxVal, &minLoc, &maxLoc);

    if (minVal <= treshold){
        res.detected = true;
        res.type = templName;
        res.loc = Point(minLoc.x+zone.x, minLoc.y+zone.y);
        res.templ = Point(templ.cols, templ.rows);
        return res;
    }
    return res;
}

/**
 *
 * @param image notre image
 * @param zone la zone de detection du texte
 * @return un detectionStruct contenant les informations (position,type,...) du texte detecté dans notre zone de detection
 */
DetectionStruct iconDetection(Mat image, Rect zone){
    DetectionStruct res;
    for (map<string,double>::iterator i = icons.begin(); i != icons.end(); i++){
        res = templateDetection(image, zone, i->first, i->second);
        if (res.detected)
            return res;
    }
    return res;
}

/**
 *
 * @param image notre image
 * @param zone la zone de detection du texte
 * @return un detectionStruct contenant les informations (position,type,...) du texte detecté dans notre zone de detection
 */
DetectionStruct sizeDetection(Mat image, Rect zone){
    DetectionStruct res;
    for (map<string,double>::iterator size = sizeIcons.begin(); size != sizeIcons.end(); size++){
        res = templateDetection(image, zone, size->first, size->second);
        if (res.detected)
            return res;
    }
    return res;
}

/**
 *
 * @param image notre image
 * @param zone la zone de detection de notre croix
 * @return un detectionStruct contenant les informations (position,type,...) de la croix detecté dans notre zone de detection
 */
DetectionStruct crossDetection(Mat image, Rect zone){
    return templateDetection(image, zone, "cross", 30000000);
}

/**
 *
 * @param image notre image
 * @param detect la zone de detection de notre icone
 * @param color la couleur du texte
 * @return notre image avec le rectangle et le nom de l'icone detecté
 */
Mat addInfoOnImage(Mat image, DetectionStruct detect, Scalar_<double> color){
    // Ajout du rectangle dans image autour du picto détecté
    rectangle( image,
               Point(detect.loc.x, detect.loc.y),
               Point(detect.loc.x + detect.templ.x, detect.loc.y + detect.templ.y),
               color, 2, 8, 0 );
    // Ajout du texte descriptif du picto détecté dans image
    putText(image, detect.type,
            Point(detect.loc.x, detect.loc.y - 10),
            FONT_HERSHEY_COMPLEX, 1, color);
    // Retourne l'image
    return image;
}

/**
 *
 * @param image notre image
 * @param zone les coordonnées du rectangle
 * @param zoneName le nom du rectangle
 * @param color la couleur du rectangle
 * @return notre image avec le rectangle
 */
Mat addInfoOnImage(Mat image, Rect zone, string zoneName, Scalar_<double> color){
    rectangle(image,zone, color,2);
    putText(image, zoneName, Point(zone.x+15,zone.y+30), FONT_HERSHEY_COMPLEX, 1, color);
    return image;
}

/**
 *
 * @param image notre image
 * @param decalageX le decalage en x de notre image
 * @param decalageY le decalage en y de notre image
 * @return une image qui est le decalage en x,y de notre image mise en entrée
 */
Mat translate(Mat image, float decalageX, float decalageY){
    Mat res;
    float warp_values[] = { 1.0, 0.0, decalageX, 0.0, 1.0, decalageY };
    Mat translation_matrix = Mat(2, 3, CV_32F, warp_values);
    warpAffine(image, res, translation_matrix, image.size());
    return res;
}

/**
 *
 * @param c le point de detection de notre croix
 * @return l'angle de rotation necessaire pour remettre l'image droite
 */
double getAngleABC(Point c) {
    Point cb = { TOPCROSS_X - c.x, TOPCROSS_Y - c.y };
    float dot = (ab.x * cb.x + ab.y * cb.y); // dot product
    float cross = (ab.x * cb.y - ab.y * cb.x); // cross product
    float alpha = atan2(cross, dot);
    return alpha * 180 / M_PI;
}

/**
 *
 * @param image notre image
 * @param point le point de detection de notre croix
 * @return une image qui est la rotation de l'image prise en paramètre avec un certain angle
 */
Mat rotate (Mat image, Point point){
    double angle = getAngleABC(point);
    Mat rotation_matix = getRotationMatrix2D(Point(TOPCROSS_X, TOPCROSS_Y), angle, 1.0);
    Mat rotated_image;
    warpAffine(image, rotated_image, rotation_matix, image.size());
    return rotated_image;
}

/**
 *
 * @param image notre image
 * @param ligne la ligne dans notre image
 * @param colonne la colonne dans notre ilage
 * @return une image qui est l'imagette contenu a la ligne ligne et colonne colonne
 */
Mat extractImagette(Mat image, int ligne, int colonne){
    int x = IMAGETTE_X + colonne*(IMAGETTE_T + 104);
    int y = IMAGETTE_Y + ligne*(IMAGETTE_T + 103);
    Rect imagetteRegion(x, y,IMAGETTE_T, IMAGETTE_T);
    return image(imagetteRegion);
}

/**
 * Crée le fichier txt contenant toutes les informations demandés.
 * @param imagette l'imagette
 * @param imagetteName le nom de l'imagette
 * @param candidat le numero de repertoire
 * @param sample le numero de fichier
 * @param ligne le numero de la ligne
 * @param colonne le cnumero de la colonne
 * @param icon un detectionStruct contenant les informations sur le type de l'icone
 * @param iconSize un detectionStruct contenant les informations sur le taille de l'icone
 */
void makeFichierTexte(Mat imagette, string imagetteName, int candidat, int sample, int ligne, int colonne, DetectionStruct icon, DetectionStruct iconSize){
    ofstream flux(repoDest + "Imagettes/" + imagetteName + ".txt");
    if(flux) {
        flux << "# Equipe : Nour Zbitou, Rodrigue Bourel, Melissande Mélissande, Arthur Moulinier; Option : Data Science;" << endl;
        flux << "label " << icon.type << endl;
        flux << "form " << intTo3digits(candidat) << intTo2digits(sample) << endl;
        flux << "scripter " <<  intTo3digits(candidat) << endl;
        flux << "page " << intTo2digits(sample) << endl;
        flux << "row " << ligne << endl;
        flux << "column " << colonne << endl;
        if (iconSize.detected) {
            flux << "size " << iconSize.type << endl;
        } else {
            flux << "size" << endl;
        }
    } else {
        cout << "ERREUR: Impossible d'ouvrir le fichier." << endl;
    }
    flux.close();  //On referme le fichier
}

/**
 *
 * @param imagette le nombre d'imagette non traiter
 * @param picto le nombre de picto non detecté
 * @param page le nombre de page non traité
 */
void MakeFichierErreur(double imagette,double picto,double page){
    //Jeu de test
    //double nb = 12;
    //Jeu de training
    double nb = NB_SAMPLES * NB_CANDIDATS;
    double pages_ = (page/nb)*100;
    double picto_ = (picto/(nb*7))*100;
    double imagette_ = (imagette/(nb*7*5))*100;

    ofstream flux(repoDest + "Erreur.txt");
    if(flux){
        flux<<"Fichier contenant le taux d'erreurs"<<endl;
        flux<<"Taux d'erreurs sur les pages " <<  pages_ << "%"<< endl;
        flux << "nombre de pages non utilisables : " << page << endl;
        flux<<"taux d'erreurs sur les picto " << picto_ << "%" << endl;
        flux << "nombre de pictogrammes non utilisables : " << picto << endl;
        flux <<"taux derreurs sur les imagettes " << imagette_ << "%" <<endl;
        flux << "nombre d'imagettes non utilisables : " << imagette << endl;
    }
    flux.close();
}

/**
 *
 * @param topCross un detectionStruct de la croix du haut
 * @param botCross un detectionStruct de la croix du bas
 * @return un double étant le resize a effectuer sur notre image pour standardisé la taille des carrés
 */
double calculRapport(DetectionStruct topCross, DetectionStruct botCross) {
    int dX = topCross.loc.x-botCross.loc.x;
    int dY = botCross.loc.y-topCross.loc.y;
    double distance = sqrt(pow(dX, 2) + pow(dY, 2));
    return 3380.237418/distance;
}

/**
 *
 * @param image notre image
 * @param rapport le rognage a effectuer sur les bords
 * @return une image rogné pour correspondre a la taille standardisé de nos images.
 */
Mat rognage (Mat image, double rapport){
    int x = abs(rapport-1)*WIDTH;
    int y = abs(rapport-1)*HEIGHT;
    //cout << x << " " << y << endl;
    Rect vraiImage(x, y,WIDTH, HEIGHT);
    return image(vraiImage);
}


int main (void) {
    string imageName = "";
    string fullName = repoSource + imageName;

    int candidat;
    int sample;
    int compteurImagette=0;
    int compteurCross=0;
    int compteurPicto=0;

    createDir();

    // parcours de toute les images de la database
    for (candidat = 0; candidat < NB_CANDIDATS; candidat++){
        for (sample = 0; sample < NB_SAMPLES; sample++){
            imageName =  intTo3digits(candidat) + intTo2digits(sample) + ".png";
            fullName = repoSource + imageName;

            Mat im = uploadImage(fullName);
            cout << imageName << endl;

            // Zone dans laquelle on va chercher la croix du haut et détection de la croix
            Rect topCrossRegion(0, 0, im.cols, CROSSREGION_H);
            DetectionStruct topCross = crossDetection(im, topCrossRegion);
            // Zone dans laquelle on va chercher la croix du bas et détection de la croix
            Rect botCrossRegion(0, im.rows-CROSSREGION_H, im.cols, CROSSREGION_H);
            DetectionStruct botCross = crossDetection(im, botCrossRegion);
            // Calcule du rapport de conversion
            if (topCross.detected && botCross.detected){
                double rapport = calculRapport(topCross, botCross);
                if (abs(rapport-1) > 0.01){
                    // reshape de la taille de l'image
                    im = augmente(im, rapport);
                    // rognage de l'image
                    im = rognage(im, rapport);
                }
            }
            else{
                compteurImagette+=35;
                compteurCross++;
                compteurPicto+=7;
                cerr << "cross not detected" << endl;
            }


            if(topCross.detected && botCross.detected) {

                // Détection de la croix du haut
                // im = addInfoOnImage(im, topCrossRegion, "top cross", CV_RGB(0,255,0));
                topCross = crossDetection(im, topCrossRegion);
                if (topCross.detected) {
                    im = addInfoOnImage(im, topCross, CV_RGB(0, 255, 0));
                    // Translation de l'image
                    im = translate(im, TOPCROSS_X - topCross.loc.x, TOPCROSS_Y - topCross.loc.y);
                }


                // Détection de la croix du bas
                botCross = crossDetection(im, botCrossRegion);
                //im = addInfoOnImage(im, botCrossRegion, "bottom cross", CV_RGB(0,255,0));
                if (botCross.detected) {
                    im = addInfoOnImage(im, botCross, CV_RGB(0, 255, 0));
                    // Rotation de l'image
                    im = rotate(im, botCross.loc);
                }
                if(!(topCross.detected&&botCross.detected)){
                    compteurImagette+=35;
                    compteurCross++;
                    compteurPicto+=7;
                    cerr << "cross not detected" << endl;
                }


                //Pour chaque lignes
                for (int ligne = 0; ligne < ROWS; ligne++) {
                    int icon_y = ICON_Y + (ligne * ICON_H);

                    // Zone dans laquelle on va chercher l'icone en début de ligne
                    Rect iconRegion(ICON_X, icon_y, ICON_W, ICON_H);
                    im = addInfoOnImage(im, iconRegion, "icon_" + to_string(ligne), CV_RGB(0, 0, 0));

                    // Détection de la taille de l'icone
                    DetectionStruct iconSize = sizeDetection(im, iconRegion);
                    if (iconSize.detected) {
                        im = addInfoOnImage(im, iconSize, CV_RGB(255, 0, 0));
                    }

                    // Détection de l'icone
                    DetectionStruct icon = iconDetection(im, iconRegion);
                    if (icon.detected){
                        im = addInfoOnImage(im, icon, CV_RGB(0,0,255));

                        // Pour chaque imagettes de la ligne
                        for (int colonne = 0; colonne<COLS; colonne++){
                            string imagetteName = icon.type + "_" + intTo3digits(candidat) + "_" + intTo2digits(sample) + "_" + to_string(ligne) + "_" + to_string(colonne);
                            // extraction de l'imagette
                            Mat imagette = extractImagette(im, ligne, colonne);
                            // enregistrement de l'imagette
                            imwrite(repoDest + "Imagettes/" + imagetteName + ".png", imagette);
                            // création du .txt
                            makeFichierTexte(imagette, imagetteName, candidat, sample, ligne, colonne, icon, iconSize);
                        }
                    }
                    //Icone non detecté donc impossible de donner le type des dessins
                    else{
                        compteurPicto++;
                        compteurImagette+=5;
                        cerr<<"Picto manquant"<<endl;

                    }
                }

                // Enregistrement ddu visu de la fiche traité dans Final
                imwrite(repoDest + "Final/" + imageName, im);
            }

        }
    }

    MakeFichierErreur(compteurImagette,compteurPicto,compteurCross);

    //termine le programme lorsqu'une touche est frappee
    waitKey(0);
    //system("pause");
    return EXIT_SUCCESS;
}