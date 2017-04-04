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

// viewer
#include "QLightManipulator.h"
// std
#include <string>
#include <sstream>

namespace ospray {
  namespace viewer {
    
    QDoubleSpinBox *newQDoubleSpinBox(double val)
    {
        QDoubleSpinBox *spin = new QDoubleSpinBox;
        spin->setValue(val);
        return spin;
    }

    QLightManipulator::QLightManipulator(std::shared_ptr<sg::Renderer> renderer, vec3f up)
    {
      sgRenderer = renderer;
      upVector = up;

      lightInfo.ospLight = ospNewLight(sgRenderer->integrator->ospRenderer, "DirectionalLight");
      lightInfo.color = vec3f(1.f, 1.f, 1.f);
      lightInfo.direction = vec3f(0.f, -1.f, 0.f);
      lightInfo.intensity = 1.f;
      ospCommit(lightInfo.ospLight);
      
      OSPData lightArray = ospNewData(1, OSP_OBJECT, &lightInfo.ospLight, 0);
      ospSetData(sgRenderer->integrator->ospRenderer, "lights", lightArray);
      ospCommit(sgRenderer->integrator->ospRenderer);
      
      //Set up all Qt bells and whistles
      QVBoxLayout *layout = new QVBoxLayout; layout->setContentsMargins(0, 0, 0, 0);
      setLayout(layout);
      intensityLabel = new QLabel(tr("Intensity:"));
      colorLabel = new QLabel(tr("Light Color (RGB):"));
      directionVectorLabel = new QLabel(tr("Light Direction Vector (XYZ):"));
      upVectorLabel = new QLabel(tr("World Up Vector (XYZ):"));

      layout->addWidget(intensityLabel);
      intensityValue = newQDoubleSpinBox(1.0);
      layout->addWidget(intensityValue);
      
      colorRedValue = newQDoubleSpinBox(1.0);
      colorGreenValue = newQDoubleSpinBox(1.0);
      colorBlueValue = newQDoubleSpinBox(1.0);
      layout->addWidget(colorLabel);
      layout->addWidget(colorRedValue);
      layout->addWidget(colorGreenValue);
      layout->addWidget(colorBlueValue);
      
      directionVectorXValue = newQDoubleSpinBox(0.0);
      directionVectorYValue = newQDoubleSpinBox(-1.0);
      directionVectorZValue = newQDoubleSpinBox(0.0);
      
      layout->addWidget(directionVectorLabel);
      layout->addWidget(directionVectorXValue);
      layout->addWidget(directionVectorYValue);
      layout->addWidget(directionVectorZValue);
      
      //Need to make these read only
      //Need to make them update with the up vector of the camera
      upVectorXValue = newQDoubleSpinBox(up.x);
      upVectorYValue = newQDoubleSpinBox(up.y);
      upVectorZValue = newQDoubleSpinBox(up.z);

      layout->addWidget(upVectorLabel);
      layout->addWidget(upVectorXValue); upVectorXValue->setReadOnly(true);
      layout->addWidget(upVectorYValue); upVectorYValue->setReadOnly(true);
      layout->addWidget(upVectorZValue); upVectorZValue->setReadOnly(true);
      
      //Add a spacer taking up the remaining vertical area so that thigns aren't oddly separated.
      layout->addStretch();
      
      applyButton = new QPushButton("Apply");
      layout->addWidget(applyButton);
      
      connect(applyButton, SIGNAL(clicked()), this, SLOT(apply()));
      
      apply();
    }

    QLightManipulator::~QLightManipulator()
    {
    }
    
    void QLightManipulator::apply() {
      ospSet3f(lightInfo.ospLight,
               "direction",
               directionVectorXValue->text().toFloat(),
               directionVectorYValue->text().toFloat(),
               directionVectorZValue->text().toFloat() );

      ospSet3f(lightInfo.ospLight,
               "color",
               colorRedValue->text().toFloat(),
               colorGreenValue->text().toFloat(),
               colorBlueValue->text().toFloat() );
      
      ospSet1f(lightInfo.ospLight,
               "intensity",
               intensityValue->text().toFloat());
      
      ospCommit(lightInfo.ospLight);
      
      emit lightsChanged();
    }
    
  } // ::viewer
} // ::ospray

