#define WIN32

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
	if (!fin)
	{
		std::cout << "buckball.raw not found" << std::endl;
		return 0;
	}

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
	osg::Geode* gnode = new osg::Geode;
	osg::Geometry* geom = new osg::Geometry;
	gnode->addDrawable(geom);

	osg::Vec3Array* vertex = new osg::Vec3Array;
	geom->setVertexArray(vertex);
	vertex->push_back(osg::Vec3(1.0, 1.0, 1.0));
	vertex->push_back(osg::Vec3(0.0, 1.0, 1.0));
	vertex->push_back(osg::Vec3(1.0, 0.0, 1.0));
	vertex->push_back(osg::Vec3(0.0, 0.0, 1.0));
	vertex->push_back(osg::Vec3(1.0, 1.0, 0.0));
	vertex->push_back(osg::Vec3(0.0, 1.0, 0.0));
	vertex->push_back(osg::Vec3(0.0, 0.0, 0.0));
	vertex->push_back(osg::Vec3(1.0, 0.0, 0.0));

	osg::DrawElementsUShort* drawElements = new osg::DrawElementsUShort(GL_TRIANGLE_STRIP);
	drawElements->push_back(3);
	drawElements->push_back(2);
	drawElements->push_back(6);
	drawElements->push_back(7);
	drawElements->push_back(4);
	drawElements->push_back(2);
	drawElements->push_back(0);

	drawElements->push_back(3);
	drawElements->push_back(1);
	drawElements->push_back(6);
	drawElements->push_back(5);
	drawElements->push_back(4);
	drawElements->push_back(1);
	drawElements->push_back(0);
	geom->addPrimitiveSet(drawElements);

	osg::StateSet* ss = gnode->getOrCreateStateSet();
	ss->setTextureAttributeAndModes(0, Create3DTextureFromRaw(), osg::StateAttribute::ON);
	ss->setMode(GL_BLEND, osg::StateAttribute::ON);

	osg::Program* program = new osg::Program;
	ss->setAttribute(program);
	program->addShader(osgDB::readRefShaderFile(osg::Shader::VERTEX, "v0.vert"));
	program->addShader(osgDB::readRefShaderFile(osg::Shader::FRAGMENT, "v0.frag"));

	ss->addUniform(new osg::Uniform("stepSize", float(1.0f / VSIZE)));
	ss->addUniform(new osg::Uniform("baseTexture", int(0)));

	return gnode;
}

int main()
{
	osgViewer::Viewer viewer;
	viewer.setSceneData(CreateCube());
	return viewer.run();
}