#include "devmem.h"

int main(){
    unsigned int ch = 0;
    unsigned int ch_sw;
    unsigned int data;
    unsigned int count;

    printf(" congrats\n");
    // try cdevmem
    //de = devmem.DevMem(HPS_LW_ADRS_OFFSET, ADC_ADDRES_OFFSET+0x8, "/dev/mem")
    // Set meassure number for ADC convert
    devmem devmemd = make_devman(HPS_LW_ADRS_OFFSET, ADC_ADDRES_OFFSET+0x8 , "/dev/mem");
    //de.write(ADC_ADDRES_OFFSET+ADC_DATA_REG_OFFSET,[FIFO_SIZE])

    //unsigned int bitset = 15;
    //bool setbit = true;
    //devmemwritebit(&devmemd, ADC_ADDRES_OFFSET+ADC_CMD_REG_OFFSET, bitset, setbit);
    //usleep(1);

    int fifo_size = FIFO_SIZE;
    devmemwrite(&devmemd, ADC_ADDRES_OFFSET+ADC_DATA_REG_OFFSET, &fifo_size);

    // # Enable the convention with the selected Channel
    // de.write(ADC_ADDRES_OFFSET+ADC_CMD_REG_OFFSET, [(ch <<1) | 0x00])
    ch_sw = ch << 1 | 0x00;
    devmemwrite(&devmemd, ADC_ADDRES_OFFSET+ADC_CMD_REG_OFFSET, &ch_sw);

    // de.write(ADC_ADDRES_OFFSET+ADC_CMD_REG_OFFSET, [(ch <<1) | 0x01])
    ch_sw = ch << 1 | 0x01;
    devmemwrite(&devmemd, ADC_ADDRES_OFFSET+ADC_CMD_REG_OFFSET, &ch_sw);
    usleep(1);

    // de.write(ADC_ADDRES_OFFSET+ADC_CMD_REG_OFFSET, [(ch <<1) | 0x00])
    ch_sw = ch << 1 | 0x00;
    devmemwrite(&devmemd, ADC_ADDRES_OFFSET+ADC_CMD_REG_OFFSET, &ch_sw);

    devmembuffer cresultlist = devmemread(&devmemd, ADC_ADDRES_OFFSET+ADC_CMD_REG_OFFSET,1);
    while (((unsigned int)list_pop(cresultlist.data)) & (1<<0)){
        count += 1;
        printf("its counting ... %d\n", count);
    };

    // # calculate the average of the FIFO
    unsigned int rawValue = 0;
    devmembuffer resultlist = devmemread(&devmemd, ADC_ADDRES_OFFSET+ADC_DATA_REG_OFFSET,1);
    for (int i; i<FIFO_SIZE; i++){
        rawValue = (unsigned int)list_pop(resultlist.data);
        printf("rawValue is %u\n", rawValue);
    }

  // value = rawValue / FIFO_SIZE

  // # Convert ADC Value to Volage
  // volage = round(value/1000,2)
  // # print the Value
  // print(str(volage))

    return 0;
}