$fn=50;
HP=10;
hauteur = 25;
diaFeet=6;
diaVis=2.6;
longueur= 127.3;
epaisseur = 1.5;
largeur = 50;


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
   translate([ 0,epaisseur,hauteur-epaisseur]) cube([longueur,largeur,epaisseur]);
}
