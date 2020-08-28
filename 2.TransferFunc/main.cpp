#include <osgViewer/viewer>
#include <osgDB/ReadFile>
#include <osg/Texture3D>
#include <osgDB/FileUtils>
#include <osg/Billboard>
#include <osg/TexGenNode>
#include <osg/ClipNode>


#ifdef _DEBUG
#pragma comment(lib, "osgViewerd.lib")
#pragma comment(lib, "osgDBd.lib")
#pragma comment(lib, "osgd.lib")
#else
#pragma comment(lib, "osgViewer.lib")
#pragma comment(lib, "osgDB.lib")
#pragma comment(lib, "osg.lib")
#endif

#define VSIZE 128
//从raw文件中创建3D纹理
osg::Texture3D* Create3DTextureFromRaw()
{
	osg::Texture3D* t3d = new osg::Texture3D;
	osg::Image* image = new osg::Image;
	image->allocateImage(VSIZE, VSIZE, VSIZE, GL_RGBA, GL_UNSIGNED_BYTE);
	t3d->setImage(image);

	osgDB::ifstream fin("buckball.raw", osgDB::ifstream::binary);

	for (int s = 0; s < VSIZE; s++)
	{
		for (int t = 0; t < VSIZE; t++)
		{
			for (int r = 0; r < VSIZE; r++)
			{
				unsigned char* datePtr = image->data(s, t, r);
				unsigned char dataTemp;
				fin.read((char*)(&dataTemp), 1);

				if (dataTemp < 10)
				{
					datePtr[0] = 0;
					datePtr[1] = 255;
					datePtr[2] = dataTemp;
					datePtr[3] = 0;
				}
				else if (dataTemp < 30)
				{
					datePtr[0] = dataTemp;
					datePtr[1] = 255;
					datePtr[2] = dataTemp;
					datePtr[3] = 10;
				}
				else if (dataTemp >= 150)
				{
					datePtr[0] = 255;
					datePtr[1] = 255;
					datePtr[2] = dataTemp;
					datePtr[3] = 100;
				}
				else
				{
					datePtr[0] = 0;
					datePtr[1] = 0;
					datePtr[2] = 0;
					datePtr[3] = 0;
				}


			}
		}
	}

	fin.close();

	t3d->setFilter(osg::Texture::MIN_FILTER, osg::Texture::NEAREST);
	t3d->setFilter(osg::Texture::MAG_FILTER, osg::Texture::NEAREST);
	t3d->setWrap(osg::Texture::WRAP_R, osg::Texture3D::CLAMP_TO_EDGE);
	t3d->setWrap(osg::Texture::WRAP_T, osg::Texture3D::CLAMP_TO_EDGE);
	t3d->setWrap(osg::Texture::WRAP_S, osg::Texture3D::CLAMP_TO_EDGE);

	return t3d;
}

//创建切片
osg::Node* CreateCube()
{
	osg::Billboard* bd = new osg::Billboard;
	bd->setMode(osg::Billboard::Mode::POINT_ROT_WORLD);


	osg::Geometry* geom = new osg::Geometry;
	bd->addDrawable(geom);
	bd->setPosition(0, osg::Vec3(0.5, 0.5, 0.5));


	osg::Vec3Array* vertex = new osg::Vec3Array;
	geom->setVertexArray(vertex);

	//
	float scalef = 1.75;
	for (int y = VSIZE - 1; y >= 0; y--)
	{
		//压入顶点
		vertex->push_back(osg::Vec3f(-0.5, y * 1.0 / (VSIZE - 1) - 0.5, -0.5) * scalef);
		vertex->push_back(osg::Vec3f(-0.5, y * 1.0 / (VSIZE - 1) - 0.5, 0.5) * scalef);
		vertex->push_back(osg::Vec3f(0.5, y * 1.0 / (VSIZE - 1) - 0.5, 0.5) * scalef);
		vertex->push_back(osg::Vec3f(0.5, y * 1.0 / (VSIZE - 1) - 0.5, -0.5) * scalef);

	}

	geom->addPrimitiveSet(new osg::DrawArrays(GL_QUADS, 0, vertex->size()));


	osg::TexGenNode* tn = new osg::TexGenNode();
	tn->addChild(bd);
	tn->setTextureUnit(0);
	tn->getTexGen()->setPlanesFromMatrix(osg::Matrix::identity());
	tn->getTexGen()->setMode(osg::TexGen::Mode::EYE_LINEAR);

	osg::StateSet* ss = tn->getOrCreateStateSet();
	ss->setTextureAttributeAndModes(0, Create3DTextureFromRaw(), osg::StateAttribute::ON);
	ss->setMode(GL_BLEND, osg::StateAttribute::ON);
	ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

	ss->setTextureMode(0, GL_TEXTURE_GEN_S, osg::StateAttribute::ON);
	ss->setTextureMode(0, GL_TEXTURE_GEN_T, osg::StateAttribute::ON);
	ss->setTextureMode(0, GL_TEXTURE_GEN_R, osg::StateAttribute::ON);

	osg::ClipNode* cn = new osg::ClipNode;
	cn->addChild(tn);

	osg::Vec3d v000 = osg::Vec3d(0.0, 0.0, 0.0);
	osg::Vec3d v100 = osg::Vec3d(1.0, 0.0, 0.0);
	osg::Vec3d v010 = osg::Vec3d(0.0, 1.0, 0.0);
	osg::Vec3d v110 = osg::Vec3d(1.0, 1.0, 0.0);

	osg::Vec3d v001 = osg::Vec3d(0.0, 0.0, 1.0);
	osg::Vec3d v101 = osg::Vec3d(1.0, 0.0, 1.0);
	osg::Vec3d v011 = osg::Vec3d(0.0, 1.0, 1.0);
	osg::Vec3d v111 = osg::Vec3d(1.0, 1.0, 1.0);

	cn->addClipPlane(new osg::ClipPlane(0, osg::Plane(v000, v011, v001)));
	cn->addClipPlane(new osg::ClipPlane(1, osg::Plane(v100, v111, v110)));
	cn->addClipPlane(new osg::ClipPlane(2, osg::Plane(v000, v101, v100)));
	cn->addClipPlane(new osg::ClipPlane(3, osg::Plane(v110, v011, v010)));
	cn->addClipPlane(new osg::ClipPlane(4, osg::Plane(v000, v110, v010)));
	cn->addClipPlane(new osg::ClipPlane(5, osg::Plane(v001, v111, v101)));

	return cn;
}

int main()
{
	osgViewer::Viewer viewer;
	viewer.setSceneData(CreateCube());
	return viewer.run();
}