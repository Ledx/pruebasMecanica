/******************************************************************************/
/* Universidad Nacional Autónoma de México                                    */
/* Instituto de Ciencias Nucleares                                            */
/*----------------------------------------------------------------------------*/
/* Software de pruebas para motores y ruedas                                  */
/*----------------------------------------------------------------------------*/
/* Desarrollador: Ing. Led Eduardo Ruvalcaba Tamés                            */
/*----------------------------------------------------------------------------*/
/* Programa: COLMENA                                                          */
/* Director: Dr. Gustavo Adolfo Medina Tanco                                  */
/*----------------------------------------------------------------------------*/
/* Actualizado                                                                */
/******************************************************************************/


/*----------------------------------------------------------------------------*/
/*Cabeceras generales                                                         */
/*----------------------------------------------------------------------------*/

/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/runtime/System.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>

/* Driver Header files */
#include <ti/drivers/ADC.h>
#include <ti/drivers/PIN.h>
#include <ti/drivers/pin/PINCC26XX.h>

/* Example/Board Header files */
#include "Board.h"

/* ADC sample count */
#define ADC_SAMPLE_COUNT  (10)

/*Task Specific defines */
#define TASKSTACKSIZE     (768)

Task_Struct task0Struct;
Char task0Stack[TASKSTACKSIZE];

Task_Struct task1Struct;
Char task1Stack[TASKSTACKSIZE];

/* ADC conversion result variables */
uint16_t adcValue0;
uint16_t adcValue1[ADC_SAMPLE_COUNT];

/*
 *  ======== taskFxn0 ========
 *  Open an ADC instance and get a sampling result from a one-shot conversion.
 */
Void taskFxn0(UArg arg0, UArg arg1)
{
    ADC_Handle   adc;
    ADC_Params   params;
    int_fast16_t res;

    ADC_Params_init(&params);
    adc = ADC_open(Board_ADC0, &params);

    if (adc == NULL) {
        System_abort("Error initializing ADC channel 0\n");
    }
    else {
        System_printf("ADC channel 0 initialized\n");
    }

    /* Blocking mode conversion */
    res = ADC_convert(adc, &adcValue0);

    if (res == ADC_STATUS_SUCCESS) {
        System_printf("ADC channel 0 convert result: 0x%x\n", adcValue0);
    }
    else {
        System_printf("ADC channel 0 convert failed\n");
    }

    ADC_close(adc);

    System_flush();
}

/*
 *  ======== taskFxn1 ========
 *  Open a ADC handle and get a array of sampling results after
 *  calling several conversions.
 */
Void taskFxn1(UArg arg0, UArg arg1)
{
    uint16_t     i;
    ADC_Handle   adc;
    ADC_Params   params;
    int_fast16_t res;

    ADC_Params_init(&params);
    adc = ADC_open(Board_ADC1, &params);

    if (adc == NULL) {
        System_abort("Error initializing ADC channel 1\n");
    }
    else {
        System_printf("ADC channel 1 initialized\n");
    }

    for (i = 0; i < ADC_SAMPLE_COUNT; i++) {
        res = ADC_convert(adc, &adcValue1[i]);

        if (res == ADC_STATUS_SUCCESS) {
            System_printf("ADC channel 1 convert result (%d): 0x%x\n", i,
                adcValue1[i]);
        }
        else {
            System_printf("ADC channel 1 convert failed (%d)\n", i);
        }

        System_flush();
    }

    ADC_close(adc);
}

/*
 *  ======== main ========
 */
int main(void)
{
    Task_Params taskParams;

    /* Call board init functions */
    Board_initGeneral();
    Board_initADC();

    /* Create tasks */
    Task_Params_init(&taskParams);
    taskParams.stackSize = TASKSTACKSIZE;
    taskParams.stack = &task0Stack;
    Task_construct(&task0Struct, (Task_FuncPtr)taskFxn0, &taskParams, NULL);

    Task_Params_init(&taskParams);
    taskParams.stackSize = TASKSTACKSIZE;
    taskParams.stack = &task1Stack;
    Task_construct(&task1Struct, (Task_FuncPtr)taskFxn1, &taskParams, NULL);

/*
 * The CC2650DK_7ID and CC1310DK_7XD measure an ambient light sensor in this example.
 * It is not powered by default to avoid high current consumption in other examples.
 * The code below turns on the power to the sensor.
 */
    PIN_State pinState;

    PIN_Config motor1[] = {
        PINCC26XX_DIO18| PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
        PINCC26XX_DIO19| PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
        PINCC26XX_DIO20| PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
        PINCC26XX_DIO21| PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
        PIN_TERMINATE
    };

    /* Turn on the power to the ambient light sensor */
    PIN_open(&pinState, motor1);

    System_printf("Starting the ADC Single Channel example\nSystem provider is "
        "set to SysMin.  Halt the target to view any SysMin contents in ROV.\n");

    /* SysMin will only print to the console when you call flush or exit */
    System_flush();

    BIOS_start();

    return (0);
}
