// http://www.doepfer.de/a100_man/a100m_e.htm
$fn=50;
hauteur = 30; // 35 with minitel
diaFeet=6;
diaVis=2.95;
longueur= 128.7;
epaisseur = 1.5;
panelT=2.2;
largeur = 51.4; 

/* TODO
hull feet

*/


module Minitel ()
{
    
    cylinder (h=10,d=16.5);
    translate ([0,11.4,0]) cylinder (h=10,d=3.2);
    translate ([0,-11.4,0]) cylinder (h=10,d=3.2);

}


difference(){

union(){
translate ([3,7.5+epaisseur,0])cylinder (h=hauteur-panelT,d=diaFeet);
//translate ([3,largeur-7.5,0])cylinder (h=hauteur-panelT,d=diaFeet);

//translate ([longueur-3,largeur-7.5,0])cylinder (h=hauteur-panelT,d=diaFeet);
translate ([longueur-3,7.5+epaisseur,0])cylinder (h=hauteur-panelT,d=diaFeet);
}

union(){
translate ([3,7.5+epaisseur,0])cylinder (h=hauteur-epaisseur,d=diaVis);
//translate ([3,largeur-7.5,0])cylinder (h=hauteur-epaisseur,d=diaVis);

//translate ([longueur-3,largeur-7.5,0])cylinder (h=hauteur-epaisseur,d=diaVis);
translate ([longueur-3,7.5+epaisseur,0])cylinder (h=hauteur-epaisseur,d=diaVis);
}


}

difference (){
cube([longueur,largeur+2*epaisseur,hauteur]);
translate ([epaisseur,epaisseur,epaisseur])    cube([longueur-2*epaisseur,largeur,hauteur]);
   translate([ 0,epaisseur,hauteur-panelT]) #cube([longueur,largeur,panelT]);
    //translate ([-5,largeur/2+epaisseur,12])rotate ([0,90,0]) Minitel();
}
