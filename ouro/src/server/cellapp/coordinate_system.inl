// 2017-2019 Rotten Visions, LLC. https://www.rottenvisions.com


namespace Ouroboros{

	
//-------------------------------------------------------------------------------------
INLINE CoordinateNode * CoordinateSystem::pFirstXNode() const { return first_x_coordinateNode_; }

//-------------------------------------------------------------------------------------
INLINE CoordinateNode * CoordinateSystem::pFirstYNode() const { return first_y_coordinateNode_; }

//-------------------------------------------------------------------------------------
INLINE CoordinateNode * CoordinateSystem::pFirstZNode() const { return first_z_coordinateNode_; }

//-------------------------------------------------------------------------------------
INLINE uint32 CoordinateSystem::size() const{ return size_; }

//-------------------------------------------------------------------------------------
INLINE bool CoordinateSystem::isEmpty() const 
{ 
	return first_x_coordinateNode_ == NULL && first_y_coordinateNode_ == NULL && first_z_coordinateNode_ == NULL;
}

//-------------------------------------------------------------------------------------
INLINE void CoordinateSystem::incUpdating()
{
	++updating_;
}

//-------------------------------------------------------------------------------------
INLINE void CoordinateSystem::decUpdating()
{
	--updating_;
}

//-------------------------------------------------------------------------------------

}
