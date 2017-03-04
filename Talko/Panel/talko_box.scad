// http://www.doepfer.de/a100_man/a100m_e.htm
$fn=50;
hauteur = 30;
diaFeet=6;
diaVis=2.8;
longueur= 128.3;
epaisseur = 1.5;
panelT=1.8;
largeur = 51.8; 



module Minitel ()
{
    
    cylinder (h=10,d=16.5);
    translate ([0,11.1,0]) cylinder (h=10,d=3.2);
    translate ([0,-11.1,0]) cylinder (h=10,d=3.2);

}


difference(){

union(){
translate ([3,7.5+epaisseur,0])cylinder (h=hauteur-epaisseur,d=diaFeet);
translate ([3,largeur-7.5,0])cylinder (h=hauteur-epaisseur,d=diaFeet);

translate ([longueur-3,largeur-7.5,0])cylinder (h=hauteur-epaisseur,d=diaFeet);
translate ([longueur-3,7.5+epaisseur,0])cylinder (h=hauteur-epaisseur,d=diaFeet);
}

union(){
translate ([3,7.5+epaisseur,0])cylinder (h=hauteur-epaisseur,d=diaVis);
translate ([3,largeur-7.5,0])cylinder (h=hauteur-epaisseur,d=diaVis);

translate ([longueur-3,largeur-7.5,0])cylinder (h=hauteur-epaisseur,d=diaVis);
translate ([longueur-3,7.5+epaisseur,0])cylinder (h=hauteur-epaisseur,d=diaVis);
}


}

difference (){
cube([longueur,largeur+2*epaisseur,hauteur]);
translate ([epaisseur,epaisseur,epaisseur])    cube([longueur-2*epaisseur,largeur,hauteur]);
   translate([ 0,epaisseur,hauteur-panelT]) cube([longueur,largeur,panelT]);
    translate ([-5,largeur/2+epaisseur,12])rotate ([0,90,0]) Minitel();
}
