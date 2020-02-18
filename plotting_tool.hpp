#ifndef PLOTTING_TOOL_HPP
#define PLOTTING_TOOL_HPP

#include "content_base.hpp"

namespace Ui {
class PlottingTool;
}

class PlottingTool : public ContentBase
{
	Q_OBJECT

public:
	explicit PlottingTool();
	~PlottingTool();

	void serialisation() override {
		ContentBase::serialisation();
		subclass("plot");
		synch("x", _xMin);
		synch("z", _xMax);
		synch("y", _yMin);
		synch("w", _yMax);
		synch("g", _graphs);
		synch("l", _logarithmic);
		synch("t", _lastTab);
	}

	void load() override;

private slots:
	void on_updateDataButton_clicked();

	void on_tabs_currentChanged(int index);

	void on_resize();

private:
	Ui::PlottingTool *ui;
	SERIALISABLE_REGISTER_POLYMORPHIC(ContentBase, PlottingTool, "plot");

	struct Graph : SerialisablePolymorphic {
		bool continuous = true;
		bool isX = false;
		std::string name;
		virtual std::vector<std::pair<float, float>> getPointsInRange(const std::vector<float>& x, float min, float max) = 0;
		virtual std::vector<float> getX(int xesNeeded) = 0;
		virtual bool changePoints(const std::vector<float>& newPoints) = 0;
		virtual std::optional<std::pair<float, float>> edges() = 0;
		virtual int size() = 0;
		virtual std::vector<std::string> printColumn() const = 0;
		virtual bool acceptsDataUpdate() const = 0;
		void serialisation() override {
			synch("n", name);
			synch("c", continuous);
			synch("x", isX);
		}
	};

	struct PointGraph : Graph {
		std::vector<float> points;
		void serialisation() override {
			Graph::serialisation();
			subclass("p");
			synch("p", points);
		}
		std::vector<std::pair<float, float>> getPointsInRange(const std::vector<float>& x, float min, float max) override;
		std::vector<float> getX(int xesNeeded) override;
		bool changePoints(const std::vector<float>& newPoints) override;
		std::optional<std::pair<float, float>> edges() override;
		int size() override;
		std::vector<std::string> printColumn() const override;
		bool acceptsDataUpdate() const override;
		SERIALISABLE_REGISTER_POLYMORPHIC(Graph, PointGraph, "p");
	};

	float _xMin;
	float _xMax;
	float _yMin;
	float _yMax;
	bool _logarithmic;
	std::vector<std::unique_ptr<Graph>> _graphs;
	int _lastTab;

	bool _autoResizing = false;
	bool haveX() const;
	void determineEdges();

};

#endif // PLOTTING_TOOL_HPP
