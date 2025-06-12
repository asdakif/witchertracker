default:
	g++ -o witchertracker src/main.cpp

grade:
	python3 test/grader.py ./witchertracker test-cases
