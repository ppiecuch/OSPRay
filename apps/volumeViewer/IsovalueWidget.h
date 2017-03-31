// ======================================================================== //
// Copyright 2009-2017 Intel Corporation                                    //
//                                                                          //
// Licensed under the Apache License, Version 2.0 (the "License");          //
// you may not use this file except in compliance with the License.         //
// You may obtain a copy of the License at                                  //
//                                                                          //
//     http://www.apache.org/licenses/LICENSE-2.0                           //
//                                                                          //
// Unless required by applicable law or agreed to in writing, software      //
// distributed under the License is distributed on an "AS IS" BASIS,        //
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. //
// See the License for the specific language governing permissions and      //
// limitations under the License.                                           //
// ======================================================================== //

#pragma once

#include <ospray/ospray.h>
<<<<<<< HEAD
#include "ospray/common/OSPCommon.h"
#include <QtWidgets>
=======
#include "ospcommon/vec.h"
#include <QtGui>
>>>>>>> 2f538262e100e9d952cca17787e4f7f913bca708

class IsosurfaceEditor;

class IsovalueWidget : public QWidget
{
Q_OBJECT

public:

  IsovalueWidget(IsosurfaceEditor *isosurfaceEditor);

  bool getIsovalueEnabled() { return isovalueCheckBox.isChecked(); }
  float getIsovalue() { return isovalueSpinBox.value(); }

  void setDataValueRange(ospcommon::vec2f dataValueRange);

signals:

  void isovalueChanged();

protected slots:

  void apply();

protected:

  ospcommon::vec2f dataValueRange;

  //! Indicates if the data value range has been set; we don't automatically set the isovalue after the first time it's set.
  bool dataRangeSet;

  QCheckBox isovalueCheckBox;
  QSlider isovalueSlider;
  QDoubleSpinBox isovalueSpinBox;

};
