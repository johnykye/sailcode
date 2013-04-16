#include "sketch.h"

void test_kcompare() {
    // Nothing yet
}

void setup() {
    Serial.begin(115200);

    testk = new kstring( 100 );
    mycon = new ashcon(&Serial);

    pinMode(LED_PIN, OUTPUT);

    Serial.println("Testing console");
}

void loop() {

    mycon->printf("> ");
    mycon->get_line();
    mycon->get_line_splitline();
    /*mycon->command_arg_append("This is a test");*/
    /*mycon->command_arg_append("Another string");*/

    mycon->command_arg_dump_debug();
    mycon->command_arg_clear();

    mycon->printf("Loopend. \n\r");
    /*delete mycon;*/
    /*delete testk;*/
    /*while( true ) delay( 100 );*/
}

