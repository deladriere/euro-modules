/*
use icon ?
scan button
announce real port
check why divide /2 (at home)
windows : export as win32 (even if pc run 64)
*/
import processing.serial.*;
import controlP5.*;
import java.io.*;

ControlP5 cp5;
DropdownList d1;
Textarea myTextarea;
Serial myPort;

String portName;
String fileName;
int serialListIndex;
int done;



Println console;

void setup() {

        done=#FFFFFF;

        clear();
        size(700, 400 );
        surface.setTitle("Bossa Loader Version 1.0");
        fileName="";
        cp5 = new ControlP5(this);
        frameRate(50);

        PFont pfont = createFont("orbitron.ttf",20,true); //Create a font
        ControlFont font = new ControlFont(pfont,20); //font, font-size


        myTextarea = cp5.addTextarea("txt")
                     .setPosition(100, 220)
                     .setSize(500, 150)
                     .setFont(createFont("", 10))
                     .setLineHeight(14)
                     .setColor(color(200))
                     .setColorBackground(color(0, 100))
                     .setColorForeground(color(255, 100));  
        ;


        d1 = cp5.addDropdownList("myList-d1")
             .setPosition(100, 150)
             .setSize(500, 200)
             .setHeight(210)
             .setItemHeight(40)
             .setBarHeight(50)
             .setFont(font)
             .setColorBackground(color(60))
             .setColorActive(color(255, 128))
        ;

        d1.getCaptionLabel().set("Choose Serial Port"); //set PORT before anything is selected



        portName = Serial.list()[0]; //0 as default
        myPort = new Serial(this, portName, 9600);

        // either do not set value or setbroadcast true after setting value
        cp5.addButton("Upload")
        .setBroadcast(false)
        .setValue(1) // sending a value
        .setPosition(480, 50)
        .setSize(120, 50)
        .setFont(font)
        .setBroadcast(true)
        ;

        cp5.addButton("Browse")
        .setBroadcast(false)
        .setValue(1) // sending a value
        .setPosition(100, 50)
        .setSize(120, 50)
        .setFont(font)
        .setBroadcast(true)
        ;
        console = cp5.addConsole(myTextarea);
        
         d1.clear(); //Delete all the items
                println("---");
                for (int i=0; i<Serial.list().length; i++) { // to verify this division by 2
                        d1.addItem(Serial.list()[i], i); //add the items in the list
                       println(Serial.list()[i]);
                }

}

void draw() {
        background(128);
        fill(done);
        noStroke();
        ellipse(650, 74, 50, 50);
        //text(fileName+portName, 100,130); not needed anymore
 
        //if(d1.isMousePressed()) {
        //        d1.clear(); //Delete all the items
        //        println("---");
        //        for (int i=0; i<Serial.list().length/2; i++) {
        //                d1.addItem(Serial.list()[i], i); //add the items in the list
        //               println(Serial.list()[i]);
        //        }
        //}
       
        if ( myPort.available() > 0) { //read incoming data from serial port
                println(myPort.readStringUntil('\n')); //read until new input
        }


}

void controlEvent(ControlEvent theEvent) { //when something in the list is selected
        myPort.clear(); //delete the port
        myPort.stop(); //stop the port
        if (theEvent.isController() && d1.isMouseOver()) {
                portName = Serial.list()[int(theEvent.getController().getValue())]; //port name is set to the selected port in the dropDownMeny
               // myPort = new Serial(this, portName, 1200); //Create a new connection // was 9600
                println("Serial index set to: " + theEvent.getController().getValue());
                // jld
               // myPort.stop();
                //delay(2000);
                 done=#FFFFFF; // use the function to clear the circle
        }
}

void Upload(float theValue) {
        done=#FFFFFF;  // white circle
        
         // force bootloader mode
         myPort = new Serial(this, portName, 1200); //Create a new connection // was 9600
         myPort.stop();
         delay(1000);
        String returnedValues;
        String commandToRun = "./bossac";

        //File workingDir = new File("/Users/jldela/Dropbox/processing/Bossa/");   // where to do it - should be full path
        //sketchPath("");   // where to do it - should be full path
        File workingDir = new File(sketchPath(""));
        // value to return any results

        
        println("Uploading",theValue);
        println(fileName);
        println("Workingdir");
        println(workingDir);
        println("--");
        
        String[] param1 = {"say","uploading file", fileName,portName};
        // exec(param1);
       
       //String[] param2= {"cmd"," /c","bossac.exe","-e","-w","-v","-R",fileName}; //without port on Windows
       // 
        String[] param2= {"./bossac","-e","-w","-v","-R","-p",portName,fileName}; // OSX

        println(param2);
        delay(2000);


        try {

                // complicated!  basically, we have to load the exec command within Java's Runtime
                // exec asks for 1. command to run, 2. null which essentially tells Processing to
                // inherit the environment settings from the current setup (I am a bit confused on
                // this so it seems best to leave it), and 3. location to work (full path is best)
                Process p = Runtime.getRuntime().exec((param2), null, workingDir);

                // variable to check if we've received confirmation of the command
                int i = p.waitFor();

                // if we have an output, print to screen
                if (i == 0) {

                        // BufferedReader used to get values back from the command
                        BufferedReader stdInput = new BufferedReader(new InputStreamReader(p.getInputStream()));

                        // read the output from the command
                        while ( (returnedValues = stdInput.readLine ()) != null) {

                                print(returnedValues);

                                if(returnedValues.indexOf("Verify successful") == 0)
                                {
                                      //print("sucess");
                                        done=#29F70F;

                                }

                                if(returnedValues.indexOf("failed") == 0)
                                {
                                        //print("failed");
                                        done=#F7320F;

                                }
                        }
                }

                // if there are any error messages but we can still get an output, they print here
                else {
                        BufferedReader stdErr = new BufferedReader(new InputStreamReader(p.getErrorStream()));

                        // if something is returned (ie: not null) print the result
                        while ( (returnedValues = stdErr.readLine ()) != null) {
                                println(returnedValues);
                        }
                }
        }

        // if there is an error, let us know
        catch (Exception e) {
                println("Error running command!");
                println(e);
        }


}

void Browse(float theValue) {
        println("Browse");
        selectInput("Select a file to process:", "fileSelected");

}

void fileSelected(File selection) {
        if (selection == null) {
                println("Window was closed or the user hit cancel.");
        } else {
                println("User selected " + selection.getAbsolutePath());
                fileName=selection.getAbsolutePath();
        }
}
