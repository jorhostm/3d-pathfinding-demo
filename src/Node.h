#pragma once

struct Node{

	int x;
	int y;
	int z;
	float h;
	float g;
	float f;
	Node* p;
	Node(int sx = 0, int sy = 0, int sz = 0, Node* sp = nullptr, float sh = 10000, float sg = 0) :
		x(sx), y(sy), z(sz), p(sp), h(sh), g(sg), f(sh+sg) {}

	bool operator==(const Node &n) const
	{
		return n.x == x && n.y == y && n.z == z;
	}
};

struct NodeHash
{
	std::size_t operator()(Node const& n) const noexcept
	{
		std::size_t h1 = std::hash<int>{}(n.x);
		std::size_t h2 = std::hash<int>{}(n.y);
		std::size_t h3 = std::hash<int>{}(n.z);
		return h1 ^ h2 ^ h3;
	}
};
//struct NodeCmp
//{
//	bool operator()(Node const& lhs, Node const& rhs) const noexcept
//	{
//		
//		return lhs.f < rhs.f;
//	}
//};
