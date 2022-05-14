#pragma once

int firstStepPosition (double* position, double deltaFi0, double deltaW0);

int positionINS (double* position, double* speeds, double deltaT);

int positionSNS (double* position, double * truePositiom);
