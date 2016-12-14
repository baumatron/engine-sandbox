#include "CHSISystem.h"

void CHSISystem::Draw(CCamera& camera, CHSIMainDisplayContext& context, bool systemSelected)
{
	// set up the light for the star
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	// Create light components
	float ambientLight[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float diffuseLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float specularLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	// Assign created components to GL_LIGHT0
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.25f);

	// label the system
	if(context.viewTier == CHSIMainDisplayContext::galactic)
	{
		Video.PushProjection3d();
		glLoadMatrixf((camera.GetTransformation().getInverse()).matrix);
		Video.vcout.setColor(CColor(0.0f, 0.8f, 1.0f));
		v3d planetPointProjected = Video.ProjectPoint(planets[0].GetWorldMatrix().getTranslation());
		Video.vcout.setPos(v3d(Video.TopViewportContext().area.getWidth()/2, Video.TopViewportContext().area.getHeight()/2) + planetPointProjected);
		Video.vcout << planets[0].GetName();
		Video.PopProjection();
	}


	for(int i = 0; i < planets.size(); i++)
	{	
		bool planetSelected(false);
		if((context.activePlanet == i) && systemSelected)
			planetSelected = true;

			// calculate the position of the light used for the star, which changes with each planet
			// to give better quality lighting
			float lightRadius = planets[i].GetRadius() * 4;
			v3d starVectorSystemSpace = planets[0].GetWorldMatrix().getTranslation()-planets[i].GetWorldMatrix().getTranslation();
			starVectorSystemSpace.makeUnitVector();
			v3d lightPosition = planets[i].GetWorldMatrix().getTranslation() + (starVectorSystemSpace*lightRadius);
					
				// set up the projection for the lighting position
			glMatrixMode(GL_PROJECTION);
			glPushMatrix();
			glLoadIdentity();

			if(Video.settings.getSh())
				gluPerspective(45.0f, (float)Video.TopViewportContext().area.getWidth() / (float)Video.TopViewportContext().area.getHeight(), 2.0f, 5000000.0f);

			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glLoadIdentity();
			// set up model view matrix with the camera
			glLoadMatrixf((camera.GetTransformation().getInverse()).matrix);
			
			// set the light position
			float position[] = { lightPosition.x, lightPosition.y, lightPosition.z, 1.0f };
			glLightfv(GL_LIGHT0, GL_POSITION, position);
			glMatrixMode(GL_PROJECTION);
			glPopMatrix();

			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();

		// draw the planet
		planets[i].Draw(camera, context, systemSelected, planetSelected);
	}
}

void CHSISystem::Think()
{
	for(vector<CHSIPlanet>::iterator it = planets.begin(); it != planets.end(); it++)
	{
		it->Think();
	}
}
