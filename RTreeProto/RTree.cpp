#pragma once
#include "Point.hpp"
#include "Rect.hpp"

struct RTree
{
	using value_type = std::pair<Rect, size_t>;
	static const size_t max_node_size = 16;
	struct RTIterator;
	struct INode;
	struct NodeBase {
		// Basic common elements
		explicit NodeBase(INode* parent) : parent_{parent} {}
		virtual ~NodeBase() =default;

		size_t size_ {0};
		INode* parent_;
		Rect outline_;
	};
	struct LeafNode;
	struct IndexNode;
	struct AbstractOperation {
		virtual void visit(IndexNode& n) =0;
		virtual void visit(LeafNode& n) =0;
	};
	struct INode {
		// pure abstract interface
		virtual ~INode() =default;
		virtual size_t size() const = 0;
		virtual void accept(AbstractOperation& a) =0;
		//virtual void add(const value_type& r)  =0;
		virtual INode* parent() =0;
		virtual LeafNode* first_leaf(size_t* depth) =0;
		virtual bool is_full() const { return size() == RTree::max_node_size; }
		virtual const Rect& outline() const =0;
		//virtual std::unique_ptr<INode> clone() =0;
	};
	struct IndexNode : public NodeBase, public INode {
		using value_type = std::pair<Rect, std::unique_ptr<INode>>;
		explicit IndexNode(INode* parent) : NodeBase(parent)
		{}
		virtual ~IndexNode() = default;
		virtual INode* parent() override { return parent_; }
		virtual size_t size() const override { return size_; }
		friend class AbstractOperation;
		virtual void accept(AbstractOperation& a) override { a.visit(*this); }
		virtual LeafNode* first_leaf(size_t* depth) override { 
			if (elements_.size() == 0)
				return nullptr;
			if (depth != nullptr)
				++(*depth);
			return std::get<1>(elements_[0])->first_leaf(depth); 
		}
		virtual const Rect& outline() const override { return outline_; }
		virtual bool is_full() const override { return size_ == elements_.size(); }

		using NodeBase::size_;
		using NodeBase::parent_;
		using NodeBase::outline_;
		std::array<IndexNode::value_type, RTree::max_node_size> elements_;
		
	};
	struct LeafNode : public NodeBase, public INode {
		explicit LeafNode(INode* parent) : NodeBase(parent) 
		{}
		virtual ~LeafNode() = default;
		using NodeBase::size_;
		using NodeBase::parent_;
		virtual INode* parent() override { return parent_; }
		virtual size_t size() const override { return size_; }
		friend class AbstractOperation;
		virtual void accept(AbstractOperation& a) override { a.visit(*this); }
		virtual LeafNode* first_leaf(size_t* depth) override { return this; }
		virtual const Rect& outline() const override { return outline_; }
		virtual bool is_full() const override { return size_ == elements_.size(); }
		std::array<RTree::value_type, RTree::max_node_size> elements_;
	};
	struct SplitOperation : public AbstractOperation {
		explicit SplitOperation() {}
		virtual void visit(IndexNode& n) {}
		virtual void visit(LeafNode& n) {

		}
	};
	struct AddOperation : public AbstractOperation {
		explicit AddOperation(const RTree::value_type& v, RTIterator& it) 
		: value_(v), it_(it)
		{}
		virtual void visit(IndexNode& n) {
			// TODO
			// choose sub tree
			// if it contains value_ rect
			// or choose the subtree which needs the least enlargement
			heap_.reserve(n.elements_.size());
			heap_.resize(0);
			static auto comp = [](const decltype(heap_)::value_type& a, const decltype(heap_)::value_type& b) -> bool {
				return a.first < b.first;
			};
			for(const auto& e : n.elements_) {
				auto area = e.first.area();
				auto enlarged_rect = Rect::combination(e.first, value_.first);
				float enlargement = static_cast<float>(enlarged_rect.area()) / (area > 0 ? area : 1);
				std::push_heap(heap_.begin(), heap_.end(), comp);
				if (e.first == enlarged_rect)
					break;
			}

			// update outline
			n.outline_ = Rect::combination(n.outline_, it_.current_node_->outline_);
		}
		virtual void visit(LeafNode& n) {
			if (n.is_full()) {
				// TODO split node
			} else {
				// insert here
				n.elements_[n.size()] =value_;
				it_.current_node_ = &n;
				it_.itstack_.push_back(n.size());
				// TODO n.outline_ = Rect::combination(n.outline_, std::get<0>(value_));
				n.outline_ = (n.size_ == 0)?value_.first:Rect::combination(n.outline(), value_.first);
				n.size_++;
			}
		}
		RTIterator& it_;
		const RTree::value_type& value_;
		std::vector<std::pair<float, INode*>> heap_;
	};
	using FindResult = std::vector<RTree::value_type>;
	struct FindExactOperation : public AbstractOperation {
		explicit FindExactOperation(const Point& p) : p_{p}
		{}
		auto begin() { return result_.begin(); }
		auto end() { return result_.end(); }
		virtual void visit(IndexNode& n) {
			for(const auto& e : n.elements_) {
				const auto& r = std::get<0>(e);
				if (r.contains(p_)) 
					std::get<1>(e)->accept(*this);
			}
		}
		virtual void visit(LeafNode& n) {
			for(const auto& e : n.elements_) {
				const auto& r = std::get<0>(e);
				if (r.contains(p_)) {
					result_.push_back(e);
				}
			}
		}
		FindResult result_;
		const Point& p_;
	};
	struct FindNearestOperation : public AbstractOperation {
		explicit FindNearestOperation(const Point& p) : p_{p}
		{}
		virtual void visit(IndexNode& n) {
			// TODO
		}
		virtual void visit(LeafNode& n) {
			// TODO
		}
		FindResult result_;
		const Point p_;
	};
	struct RTIterator
	{
		bool operator==(const RTIterator& i) const {
			if (current_node_ == nullptr && i.current_node_ == nullptr)
				return true;
			return std::tie(current_node_, itstack_) == std::tie(i.current_node_, i.itstack_);
		}
		bool operator!=(const RTIterator& i) const { return !(*this == i); }
		RTIterator& operator++() { 
			if (itstack_.size() > 0) {
				auto pos = itstack_.back();
				++pos;
				if (pos + 1 < current_node_->size())
					return *this;

				// search into upwards index nodes
				INode* node = current_node_;
				while (itstack_.size() > 0 && node != nullptr && node->size() <= pos) {
					itstack_.pop_back();
					pos = ++itstack_.back();
					node = node->parent();
				}
				if (itstack_.size() == 0) {
					// nothing found; we're at the end
					current_node_ = nullptr;
				} else {
					// search downward to leaf node
					size_t depth {0};
					current_node_ = node->first_leaf(&depth);
					for(size_t i = 0; i < depth; ++i) {
						itstack_.push_back(0);
					} 
				}
			}
			return *this; 
		}
		RTree::value_type& operator*() { return current_node_->elements_[itstack_.back()]; }
		//RTIterator& operator++(int) { return *this; }
		//RTree* tree_ { nullptr };
		LeafNode* current_node_ { nullptr };
		std::vector<size_t> itstack_;
	};
	RTIterator begin();
	RTIterator end() { return RTIterator(); }

	RTIterator add(const value_type& r) {
		RTIterator it;
		AddOperation ao(r, it);
		root_->accept(ao);
		return it;
	}
	FindResult find_containing(const Point& p) {
		FindExactOperation feo(p);
		root_->accept(feo);
		return feo.result_;
	}
	FindResult find_nearest(const Point& p, size_t n = 1) {
		FindNearestOperation fno(p);
		root_->accept(fno);
		return fno.result_;
	}

	std::unique_ptr<INode> root_ = std::make_unique<LeafNode>(nullptr);
};
