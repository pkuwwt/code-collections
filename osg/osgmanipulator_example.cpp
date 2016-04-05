#include <osgViewer/Viewer>
#include <osgDB/ReadFile>
#include <osgManipulator/TabBoxDragger>

#include <iostream>

osg::Group* addBoxDragger(osg::Node* scene) {
	osg::ref_ptr<osgManipulator::TabBoxDragger> dragger = new osgManipulator::TabBoxDragger;
	dragger->setupDefaultGeometry();
	float scale = scene->getBound().radius()*1.6;
	dragger->setMatrix(osg::Matrix::scale(scale,scale,scale) *
					   osg::Matrix::translate(scene->getBound().center()));
	osg::ref_ptr<osg::MatrixTransform> transform = new osg::MatrixTransform;
	transform->addChild(scene);
	osg::ref_ptr<osg::Group> root = new osg::Group;
	root->addChild(dragger);
	root->addChild(transform);
	dragger->addTransformUpdating(transform);
	dragger->setHandleEvents(true);
        dragger->setActivationModKeyMask(osgGA::GUIEventAdapter::MODKEY_CTRL);
        dragger->setActivationKeyEvent('a');
	return root.release();
}

int main(int argc, char* argv[]) {
	std::cout << "Press Ctrl to PICK and DRAG the handle" << std::endl;
	osgViewer::Viewer viewer;
	osg::ref_ptr<osg::Node> scene = osgDB::readNodeFile("cow.osg");
	viewer.setSceneData(addBoxDragger(scene.get()));
	viewer.run();
	return 0;
}
