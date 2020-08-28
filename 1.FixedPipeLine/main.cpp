#include <osgViewer/viewer>
#include <osgDB/ReadFile>
#include <osg/Texture3D>
#include <osgDB/FileUtils>

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
				datePtr[0] = dataTemp;
				datePtr[1] = 0;
				datePtr[2] = 0;
				if (dataTemp < 100)
				{
					dataTemp = 0;
				}
				datePtr[3] = dataTemp;

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
	osg::Geode* gnode = new osg::Geode;
	osg::Geometry* geom = new osg::Geometry;
	gnode->addDrawable(geom);

	osg::Vec3Array* vertex = new osg::Vec3Array;
	osg::Vec3Array* texCoord = new osg::Vec3Array;
	geom->setVertexArray(vertex);
	geom->setTexCoordArray(0, texCoord);

	//
	for (int y = VSIZE - 1; y >= 0; y--)
	{
		//压入顶点
		vertex->push_back(osg::Vec3f(-0.5, y * 1.0 / VSIZE, -0.5));
		vertex->push_back(osg::Vec3f(-0.5, y * 1.0 / VSIZE, 0.5));
		vertex->push_back(osg::Vec3f(0.5, y * 1.0 / VSIZE, 0.5));
		vertex->push_back(osg::Vec3f(0.5, y * 1.0 / VSIZE, -0.5));

		texCoord->push_back(osg::Vec3f(0.0, y * 1.0 / VSIZE, 0.0));
		texCoord->push_back(osg::Vec3f(0.0, y * 1.0 / VSIZE, 1.0));
		texCoord->push_back(osg::Vec3f(1.0, y * 1.0 / VSIZE, 1.0));
		texCoord->push_back(osg::Vec3f(1.0, y * 1.0 / VSIZE, 0.0));
	}

	geom->addPrimitiveSet(new osg::DrawArrays(GL_QUADS, 0, vertex->size()));

	osg::StateSet* ss = gnode->getOrCreateStateSet();
	ss->setTextureAttributeAndModes(0, Create3DTextureFromRaw(), osg::StateAttribute::ON);
	ss->setMode(GL_BLEND, osg::StateAttribute::ON);
	ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

	return gnode;
}

int main()
{
	osgViewer::Viewer viewer;
	viewer.setSceneData(CreateCube());
	return viewer.run();
}