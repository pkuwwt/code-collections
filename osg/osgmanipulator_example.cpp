#include <osgViewer/Viewer>
#include <osgDB/ReadFile>
#include <osgManipulator/CommandManager>
#include <osgManipulator/Selection>
#include <osgManipulator/TabBoxDragger>
#include <osgGA/GUIEventHandler>
#include <osgGA/GUIActionAdapter>

#include <iostream>

class PickModeHandler : public osgGA::GUIEventHandler
{
	public:
		enum Modes
		{
			VIEW = 0,
			PICK
		};

		PickModeHandler():
			_mode(VIEW), 
			_activeDragger(0)
	{
	}        

		bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa,
				osg::Object*, osg::NodeVisitor*)
		{
			osgViewer::View* view = dynamic_cast<osgViewer::View*>(&aa);
			if (!view) return false;

			if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Tab &&
					ea.getEventType() == osgGA::GUIEventAdapter::KEYDOWN &&
					_activeDragger == 0)
			{
				_mode = ! _mode;
				std::cout << "switch mode to " << ((_mode==VIEW) ? "VIEW" : "PICK") << std::endl;
			}

			if (VIEW == _mode) return false;

			switch (ea.getEventType())
			{
				case osgGA::GUIEventAdapter::PUSH:
					{
						std::cout << "PUSH" << std::endl;
						osgUtil::LineSegmentIntersector::Intersections intersections;

						_pointer.reset();

						if (view->computeIntersections(ea.getX(),ea.getY(),intersections))
						{
							_pointer.setCamera(view->getCamera());
							_pointer.setMousePosition(ea.getX(), ea.getY());

							for(osgUtil::LineSegmentIntersector::Intersections::iterator hitr = intersections.begin();
									hitr != intersections.end();
									++hitr)
							{
								_pointer.addIntersection(hitr->nodePath, hitr->getLocalIntersectPoint());
							}
							for (osg::NodePath::iterator itr = _pointer._hitList.front().first.begin();
									itr != _pointer._hitList.front().first.end();
									++itr)
							{
								osgManipulator::Dragger* dragger = dynamic_cast<osgManipulator::Dragger*>(*itr);
								if (dragger)
								{

									dragger->handle(_pointer, ea, aa);
									_activeDragger = dragger;
									break;
								}                   
							}
						}
					}
				case osgGA::GUIEventAdapter::DRAG:
				case osgGA::GUIEventAdapter::RELEASE:
					{
						std::cout << "DRAG or RELEASE" << std::endl;
						if (_activeDragger)
						{
							_pointer._hitIter = _pointer._hitList.begin();
							_pointer.setCamera(view->getCamera());
							_pointer.setMousePosition(ea.getX(), ea.getY());

							_activeDragger->handle(_pointer, ea, aa);
						}
						break;
					}
				default:
					break;
			}

			if (ea.getEventType() == osgGA::GUIEventAdapter::RELEASE)
			{
				_activeDragger = 0;
				_pointer.reset();
			}

			return true;
		}

	private:
		unsigned int _mode;
		osgManipulator::Dragger* _activeDragger;
		osgManipulator::PointerInfo _pointer;
};

osg::Group* addBoxDragger(osg::Node* scene, osgManipulator::CommandManager* cmdMgr) {
	osg::ref_ptr<osgManipulator::TabBoxDragger> dragger = new osgManipulator::TabBoxDragger;
	dragger->setupDefaultGeometry();
	float scale = scene->getBound().radius()*1.6;
	dragger->setMatrix(osg::Matrix::scale(scale,scale,scale)*osg::Matrix::translate(scene->getBound().center()));
	osg::ref_ptr<osgManipulator::Selection> selection = new osgManipulator::Selection;
	selection->addChild(scene);
	osg::ref_ptr<osg::Group> root = new osg::Group;
	root->addChild(dragger);
	root->addChild(selection);
	cmdMgr->connect(*dragger,*selection);
	return root.release();
}

int main(int argc, char* argv[]) {
	std::cout << "Press Tab to switch mode betwen VIEW/PICK" << std::endl;
	osgViewer::Viewer viewer;
	osg::ref_ptr<osgManipulator::CommandManager> cmdMgr = new osgManipulator::CommandManager;
	osg::ref_ptr<osg::Node> scene = osgDB::readNodeFile("cow.osg");
	viewer.setSceneData(addBoxDragger(scene.get(), cmdMgr.get()));
	viewer.addEventHandler(new PickModeHandler);
	viewer.run();
	return 0;
}
