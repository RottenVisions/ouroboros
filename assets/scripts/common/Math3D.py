import sys
import Ouroboros

"""
Obtain the intersection percentage of 2 circles. It is known that the radius of the circle A is r1, 
the radius of the circle B is r2, and the distance between the centers of the two circles is d, 
and the ratio of the area of ​​the intersection of the two circles to the area of ​​the circle A is returned.
"""
"""
inline double CalcCircularIntersectPercentage(double r1, double r2, double d)
{
	if (r1 <= 0 || r2 <= 0)
	{
		return -1;
	}

	if (d >= (r1 + r2))
	{
		return 0;
	}

	if (r1 > r2)
	{
		if (d <= (r1 - r2))
		{
			return (double)((r2*r2) / (r1*r1));
		}
	}
	else
	{
		if (d <= (r2 - r1))
		{
			return 1;
		}
	}

	double a1 = acos((r1*r1 - r2*r2 + d*d) / (2 * r1*d));
	double a2 = acos((r2*r2 - r1*r1 + d*d) / (2 * r2*d));
	double p = (r1 + r2 + d) / 2;
	double sq = 2 * sqrt(p * (p - r1) * (p - r2) * (p - d));
	double sec1 = r1*r1 * a1;
	double sec2 = r2*r2 * a2;
	double s1 = 3.1415926535898 * r1 * r1;
	double result = (sec1 + sec2 - sq) / s1;
	return result;
}
"""

class Sector(Area):
	def __init__(self, parent):
		"""
		Constructor.
		"""
		Area.__init__(self, parent)
		self.radius = 2.0
		self.angle = 120 / 2

	def load(self, dictDat):
		self.radius = dictDat["value1"]
		self.angle = dictDat["value2"] / 2

	def inArea(self, caster, entity, transmitDir):
		"""
		Whether the entity is in the fan range
		"""
		srcPos = Math.Vector3(caster.position)
		srcPos.y = 0

		desPos = Math.Vector3(entity.position)
		desPos.y = 0

		desDir = desPos - srcPos
		desDir.y = 0
		desDir.normalise()

		an = transmitDir.dot(desDir)

		if an < -1:
			an = -1

		if an == 0:  # Just in the same position as the caster
			an = 1

		if an > 1:
			an = 1

		angle = int(math.acos(an) / 3.1415926 * 180)
		if angle <= self.angle:  # Less than or equal to the angle
			return True

		return False

