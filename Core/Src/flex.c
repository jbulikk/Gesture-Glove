#include "flex.h"

void flex_assign_raw_values_to_fingers(uint16_t *data_buffer, FlexHandRaw *handStructure)
{
    handStructure->thumb = data_buffer[ORDER_THUMB];
    handStructure->index = data_buffer[ORDER_INDEX];
    handStructure->middle = data_buffer[ORDER_MIDDLE];
    handStructure->ring = data_buffer[ORDER_RING];
    handStructure->pinky = data_buffer[ORDER_PINKY];
}