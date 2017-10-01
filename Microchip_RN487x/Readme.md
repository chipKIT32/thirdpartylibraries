# Microchip_RN487x

Arduino library for using the Microchip RN487x BLE module.

chipKIT version forked from https://github.com/janvl1234/Microchip_RN487x
by Brian Schmalz for chipKIT project. 

## Usage

Quick example:

```c
#include "RN487x_BLE.h"

#define bleSerial Serial1

void setup()
{
	rn487xBle.hwInit();
	bleSerial.begin(rn487xBle.getDefaultBaudRate());
	rn487xBle.initBleStream(&bleSerial);
	if (rn487xBle.swInit())
	{
		rn487xBle.enterCommandMode();
		rn487xBle.stopAdvertising();
		rn487xBle.setAdvPower(3);
		rn487xBle.setSerializedName("Microchip");
		rn487xBle.clearAllServices();
		rn487xBle.reboot();
	}
}

void loop()
{
}

```


## Contributing


## History

v1.0.0 Initial Release

## License

(c) 2016 Microchip Technology Inc. and its subsidiaries. You may use this
    software and any derivatives exclusively with Microchip products.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
    WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

    MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
    TERMS.

