#include "histProduce/histProduce/interface/objectReadingFormatTreeArrayLbTk.h"

TLorentzVector readingObject::getP4()
{
    TLorentzVector obj;
    obj.SetPtEtaPhiM(data->readD[formatTreeArray_LbTk::readVarD::lbtkPt][idx],data->readD[formatTreeArray_LbTk::readVarD::lbtkEta][idx],data->readD[formatTreeArray_LbTk::readVarD::lbtkPhi][idx],data->readD[formatTreeArray_LbTk::readVarD::lbtkMass][idx] );
    obj.SetPtEtaPhiM(data->readD[formatTreeArray_LbTk::readVarD::tktkPt][idx],data->readD[formatTreeArray_LbTk::readVarD::tktkEta][idx],data->readD[formatTreeArray_LbTk::readVarD::tktkPhi][idx],data->readD[formatTreeArray_LbTk::readVarD::tktkMass][idx] );
    return obj;
}
