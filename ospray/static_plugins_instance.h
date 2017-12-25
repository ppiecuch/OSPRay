#ifdef OSP_PLUGIN_INSTANCE
	OSP_PLUGIN_INSTANCE(ao);
	OSP_PLUGIN_INSTANCE(ao1);
	OSP_PLUGIN_INSTANCE(ao2);
	OSP_PLUGIN_INSTANCE(ao4);
	OSP_PLUGIN_INSTANCE(ao8);
	OSP_PLUGIN_INSTANCE(ao16);

	OSP_PLUGIN_INSTANCE(OBJ);
	OSP_PLUGIN_INSTANCE(obj);
	OSP_PLUGIN_INSTANCE(pathtracer);
	OSP_PLUGIN_INSTANCE(pt);
	OSP_PLUGIN_INSTANCE(raycast);
	OSP_PLUGIN_INSTANCE(eyeLight);
	OSP_PLUGIN_INSTANCE(raycast_eyelight);
	OSP_PLUGIN_INSTANCE(raycast_Ng);
	OSP_PLUGIN_INSTANCE(raycast_Ns);
	OSP_PLUGIN_INSTANCE(raycast_dPds);
	OSP_PLUGIN_INSTANCE(raycast_dPdt);
	OSP_PLUGIN_INSTANCE(eyeLight_primID);
	OSP_PLUGIN_INSTANCE(primID);
	OSP_PLUGIN_INSTANCE(eyeLight_instID);
	OSP_PLUGIN_INSTANCE(instID);
	OSP_PLUGIN_INSTANCE(eyeLight_geomID);
	OSP_PLUGIN_INSTANCE(geomID);
	OSP_PLUGIN_INSTANCE(eyeLight_vertexColor);
	OSP_PLUGIN_INSTANCE(backfacing_Ng);
	OSP_PLUGIN_INSTANCE(backfacing_Ns);
	OSP_PLUGIN_INSTANCE(testFrame);
	OSP_PLUGIN_INSTANCE(raytracer);
	OSP_PLUGIN_INSTANCE(rt);
	OSP_PLUGIN_INSTANCE(scivis);

	OSP_PLUGIN_INSTANCE(orthographic);
	OSP_PLUGIN_INSTANCE(panoramic);
	OSP_PLUGIN_INSTANCE(perspective);
	OSP_PLUGIN_INSTANCE(thinlens);

	OSP_PLUGIN_INSTANCE(AmbientLight);
	OSP_PLUGIN_INSTANCE(ambient);
	OSP_PLUGIN_INSTANCE(DirectionalLight);
	OSP_PLUGIN_INSTANCE(DistantLight);
	OSP_PLUGIN_INSTANCE(distant);
	OSP_PLUGIN_INSTANCE(directional);
	OSP_PLUGIN_INSTANCE(PointLight);
	OSP_PLUGIN_INSTANCE(SphereLight);
	OSP_PLUGIN_INSTANCE(sphere);
	OSP_PLUGIN_INSTANCE(QuadLight);
	OSP_PLUGIN_INSTANCE(quad);
	OSP_PLUGIN_INSTANCE(SpotLight);
	OSP_PLUGIN_INSTANCE(ExtendedSpotLight);
	OSP_PLUGIN_INSTANCE(spot);
	
	OSP_PLUGIN_INSTANCE(OBJMaterial);
	OSP_PLUGIN_INSTANCE(PathTracer_Glass);
	OSP_PLUGIN_INSTANCE(PathTracer_Matte);
	OSP_PLUGIN_INSTANCE(PathTracer_Metal);		
	OSP_PLUGIN_INSTANCE(PathTracer_MetallicPaint);		
	OSP_PLUGIN_INSTANCE(PathTracer_OBJMaterial);		
	OSP_PLUGIN_INSTANCE(PathTracer_default);		
	OSP_PLUGIN_INSTANCE(PathTracer_Plastic);		
	OSP_PLUGIN_INSTANCE(PathTracer_ThinGlass);		
	OSP_PLUGIN_INSTANCE(PathTracer_Velvet);		

	OSP_PLUGIN_INSTANCE(cylinders);		
	OSP_PLUGIN_INSTANCE(instance);		
	OSP_PLUGIN_INSTANCE(isosurfaces);		
	OSP_PLUGIN_INSTANCE(slices);		
	OSP_PLUGIN_INSTANCE(spheres);		
	OSP_PLUGIN_INSTANCE(streamlines);		
	OSP_PLUGIN_INSTANCE(triangles);		
	OSP_PLUGIN_INSTANCE(trianglemesh);
	
	OSP_PLUGIN_INSTANCE(piecewise_linear);
	
	#ifdef OSPRAY_MODULE_LOADERS
		OSP_PLUGIN_INSTANCE(osp);
		OSP_PLUGIN_INSTANCE(raw);
		OSP_PLUGIN_INSTANCE(gz);
		OSP_PLUGIN_INSTANCE(ply);
		OSP_PLUGIN_INSTANCE(bob);
	#endif

	OSP_PLUGIN_INSTANCE(block_bricked_volume);
	OSP_PLUGIN_INSTANCE(shared_structured_volume);
#ifdef EXP_DATA_PARALLEL
	OSP_PLUGIN_INSTANCE(data_distributed_volume);
#endif
        
#endif // OSP_PLUGIN_INSTANCE
