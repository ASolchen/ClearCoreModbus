
#include "ClearCore.h"
#include "mb_tcp_server.h"

int main(void) {
    /* Replace with your application code below */
    bool ledState = true;
    while (1)
    {
        ConnectorLed.State(ledState);
        ledState = !ledState;
        Delay_ms(1000);
    }
}