#ifndef _Application_TP1_siaa_H
#define _Application_TP1_siaa_H

#include <HelperGl/Camera.h>
#include <HelperGl/LightServer.h>
#include <Application/BaseWithKeyboard.h>
#include <Application/KeyboardStatus.h>
#include <SceneGraph/Group.h>
#include <SceneGraph/MeshVBO_v2.h>
#include <GL/compatibility.h>
#include <SceneGraph/Sphere.h>
#include <HelperGl/Material.h>
#include <HelperGl/Color.h>
#include <SceneGraph/CoordinateSystem.h>
#include <SceneGraph/Translate.h>
#include <SceneGraph/BeeModel.h>
#include <SceneGraph\HermiteSpline.h>
#include <SceneGraph\Trajectoire.h>


namespace Application
{
	class TP1_siaa : public BaseWithKeyboard
	{
	protected:
		HelperGl::Camera m_camera;

		SceneGraph::Group m_root;
		SceneGraph::BeeModel * m_bee;
		HermiteSpline * m_interpolation;
		Trajectoire * m_trajectoire;
		int m_nbPointsTrajectoire;

		float m_compteur;

		virtual void handleKeys()
		{
			// The camera translation speed
			float cameraSpeed = 5.0f;
			// The camera rotation speed (currently not used)
			float rotationSpeed = float(Math::pi / 2.0);

			// quit
			if (m_keyboard.isPressed('0')) { quit(); }
			// Go front
			if (m_keyboard.isPressed('+') || m_keyboard.isPressed('=')) { m_camera.translateLocal(Math::makeVector(0.0f, 0.0f, -cameraSpeed * (float)getDt())); }
			// Go back
			if (m_keyboard.isPressed('-')) { m_camera.translateLocal(Math::makeVector(0.0f, 0.0f, (float)cameraSpeed*(float)getDt())); }
			// Go left
			if (m_keyboard.isPressed('1') || m_keyboard.isPressed('q')) { m_camera.translateLocal(Math::makeVector(-cameraSpeed * (float)getDt(), 0.0f, 0.0f)); }
			// Go right
			if (m_keyboard.isPressed('3') || m_keyboard.isPressed('d')) { m_camera.translateLocal(Math::makeVector((float)cameraSpeed*(float)getDt(), 0.0f, 0.0f)); }
			// Go down
			if (m_keyboard.isPressed('2') || m_keyboard.isPressed('s')) { m_camera.translateLocal(Math::makeVector(0.0f, -cameraSpeed * (float)getDt(), 0.0f)); }
			// Go up
			if (m_keyboard.isPressed('5') || m_keyboard.isPressed('z')) { m_camera.translateLocal(Math::makeVector(0.0f, (float)cameraSpeed*(float)getDt(), 0.0f)); }

			//turn left
			if (m_keyboard.isPressed('4') || m_keyboard.isPressed('a')) {
				m_camera.rotateLocal(Math::makeVector(0.0f, 1.0f, 0.0f), (float)cameraSpeed*(float)getDt()*0.2);
				m_camera.translateLocal(Math::makeVector((float)cameraSpeed*(float)getDt(), 0.0f, 0.0f));
			}
			//turn right
			if (m_keyboard.isPressed('6') || m_keyboard.isPressed('e')) {
				m_camera.rotateLocal(Math::makeVector(0.0f, -1.0f, 0.0f), (float)cameraSpeed*(float)getDt()*0.2);
				m_camera.translateLocal(Math::makeVector(-cameraSpeed * (float)getDt(), 0.0f, 0.0f));
			}
		}

	public:
		TP1_siaa()
		{
			//Init camera position
			m_camera.translateFront(-5);

			//Ajout du repere
			SceneGraph::CoordinateSystem * coord = new SceneGraph::CoordinateSystem(1);
			m_root.addSon(coord);

			m_bee = new SceneGraph::BeeModel;
			m_root.addSon(m_bee->getGraph());

			//interpolation
			m_compteur = 0;
			Math::Vector3f p0 = Math::makeVector(0.0, 0.0, 0.0);
			Math::Vector3f d0 = Math::makeVector(0.0, 0.0, 0.0);
			Math::Vector3f p1 = Math::makeVector(3.0, 3.0, 3.0);
			Math::Vector3f d1 = Math::makeVector(0.0, 1.0, 0.0);
			m_interpolation = new HermiteSpline(p0, p1, d0, d1);


			std::vector<Math::Vector3f> points;
			std::vector<Math::Vector3f> tangentes;
			points.push_back(Math::makeVector(0.0, 0.0, 0.0));
			points.push_back(Math::makeVector(3.0, 0.0, 0.0));
			points.push_back(Math::makeVector(3.0, 3.0, 3.0));
			points.push_back(Math::makeVector(0.0, 3.0, 3.0));
			//points.push_back(Math::makeVector(3.0, 5.0, 7.0));
			//points.push_back(Math::makeVector(0.0, 0.0, 0.0));

			tangentes.push_back(Math::makeVector(1.0, 1.0, 0.0));
			tangentes.push_back(Math::makeVector(1.0, 0.0, 1.0));
			tangentes.push_back(Math::makeVector(0.0, 1.0, 1.0));
			tangentes.push_back(Math::makeVector(1.0, 1.0, 0.0));
			//tangentes.push_back(Math::makeVector(1.0, 0.0, 0.0));

			m_trajectoire = new Trajectoire(points, tangentes);
			m_nbPointsTrajectoire = points.size();

		}

		virtual void initializeRendering()
		{
			// Light
			HelperGl::Color lightColor(1.0, 1.0, 1.0);
			HelperGl::Color lightAmbiant(0.0, 0.0, 0.0, 0.0);
			Math::Vector4f lightPosition = Math::makeVector(0.0f, 0.0f, 10000.0f, 1.0f); // Point light centered in 0,0,0
			HelperGl::LightServer::Light * light = HelperGl::LightServer::getSingleton()->createLight(lightPosition.popBack(), lightColor, lightColor, lightColor);
			light->enable();
		}

		virtual void render(double dt)
		{
			handleKeys();
			GL::loadMatrix(m_camera.getInverseTransform());
			m_root.draw();
			m_bee->rotateWings(dt, 30);
			//m_bee->moveBee(Math::makeVector(dt, dt, dt));
			

			/*** 2 points ***/
			/*
			//Position
			m_bee->setPositionBee(m_interpolation->compute(m_compteur));

			//Rotation
			Math::Vector3f speedVector = m_interpolation->getSpeed(m_compteur);
			Math::Vector3f speedVectorPolarCoord = m_interpolation->getSpeedSphericalCoord(m_compteur);
			
			m_bee->setRotationBeeY(-speedVectorPolarCoord[1]);
			m_bee->setRotationBeeZ(speedVectorPolarCoord[2]);

			m_compteur = (m_compteur+dt) - floor(m_compteur+dt);
			*/

			/*** Trajectoire ***/
			
			Math::Vector3f posBee1 = m_bee->getPositionBee();

			//Position
			m_bee->setPositionBee(m_trajectoire->computeTrajectoire(m_compteur));

			//Vitesse
			Math::Vector3f posBee2 = m_bee->getPositionBee();

			Math::Vector3f speedVect = (posBee2 - posBee1).normalized();

			float r = sqrt(pow(speedVect[0], 2.0) + pow(speedVect[1], 2.0) + pow(speedVect[2], 2.0));
			float phi = acos(speedVect[2] / r);
			float theta = atan(speedVect[1] / speedVect[0]);
			Math::Vector3f speedVectorPolarCoord = Math::makeVector(r, theta, phi);
			speedVectorPolarCoord = speedVectorPolarCoord.normalized();
			
			//Rotation
			//Math::Vector3f speedVector = m_trajectoire->getSpeedTrajectoire(m_compteur);
			//Math::Vector3f speedVectorPolarCoord = m_trajectoire->getSpeedSphericalCoordTrajectoire(m_compteur);

			m_bee->setRotationBeeY(-speedVectorPolarCoord[2]);
			m_bee->setRotationBeeZ(speedVectorPolarCoord[1]);
			
			m_compteur = fmod(m_compteur + dt, m_nbPointsTrajectoire);
			
		}
	};
}

#endif