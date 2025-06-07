#ifndef __INC_ETER_MARSHAL_H__
#define __INC_ETER_MARSHAL_H__

extern PyObject* _PyMarshal_ReadObjectFromFile(FILE* fp);
extern PyObject* _PyMarshal_ReadLastObjectFromFile(FILE* fp);
extern long _PyMarshal_ReadLongFromFile(FILE* fp);

#endif // __INC_ETER_MARSHAL_H__
