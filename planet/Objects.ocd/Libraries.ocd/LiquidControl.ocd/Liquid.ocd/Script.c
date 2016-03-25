/**
 * Liquid
 *
 * Basic interface for liquids. The logic for adding and removing liquid,
 * PutLiquid() and RemoveLiquid() is deliberately the same as in the
 * liquid container, so that scripts can be read more easily.
 *
 * Author: Marky
 */
 
#include Library_Stackable
 
static const FX_LIQUID_Dispersion = "IntLiquidDispersion";

func IsLiquid() { return true;}
func InitialStackCount(){ return 1; }
func MaxStackCount()
{
	if (this)
	{
		if (Contained() && Contained()->~IsLiquidContainer())
		{
			// Stack limit is: [what is already inside the stack] + [free space in the container].
			return GetLiquidAmount() + Contained()->~GetLiquidAmountRemaining();
		}
	}
	
	return Stackable_Max_Count;
}



// -------------- Getters
//
// Getters for stored liquid and amount
// - these should be used primarily by objects that include this library
//
// naming scheme: GetLiquid[attribute], because it concerns the liquid

func GetLiquidType()
{
	return "undefined";
}

func GetLiquidAmount()
{
	return GetStackCount();
}

// -------------- Dispersion

func Departure(object container)
{
	var fx = GetEffect(FX_LIQUID_Dispersion, this);
	if (!fx)
	{
		AddEffect(FX_LIQUID_Dispersion, this, 1, 1, this);
	}
	_inherited(...);
}

func FxIntLiquidDispersionTimer(object target, proplist fx, int timer)
{
	if (!(target->Contained()))
	{
		target->Disperse();
	}

	return FX_Execute_Kill;
}

func Disperse(int angle, int strength)
{
	// does nothing but remove the object - overload if you want special effects
	RemoveObject();
}

func DisperseMaterial(string material_name, int amount, int strength, int angle, int angle_variance)
{
	angle = angle ?? 0;
	strength = strength ?? 30;
	angle_variance = angle_variance ?? 30;
	
	CastPXS(material_name, amount, strength, 0, 0, angle, 30);
}

func DisperseParticles(string particle_name, int amount, int strength, int angle, int angle_variance, proplist template, int lifetime)
{
	angle = angle ?? 0;
	strength = strength ?? 30;
	angle_variance = angle_variance ?? 30;
	lifetime = lifetime ?? 30;
	template = template ?? Particles_Material(RGB(255, 255, 255));
	
	for (var i = 0; i < amount; ++i)
	{
		var p_strength = RandomX(strength / 2, strength);
		var p_angle = RandomX(angle - angle_variance, angle + angle_variance);
		var v_x = +Sin(p_angle, p_strength);
		var v_y = -Cos(p_angle, p_strength);
		
		CreateParticle(particle_name, 0, 0, v_x, v_y, 30, template, 1);
	}
}


// -------------- Status


func UpdateName()
{
	var container = Contained();
	
	if (container && container->~IsLiquidContainer())
	{
		SetName(Format("%d/%d %s", GetLiquidAmount(), container->GetLiquidContainerMaxFillLevel(), GetID()->GetName()));
	}
	else
	{
		SetName(Format("%dx %s", GetLiquidAmount(), GetID()->GetName()));
	}
}


// 1000 liquid items count as 1 mass unit
// this may have to be tuned or made object-specific?
func UpdateMass()
{
	SetMass(GetID()->GetMass() * Max(1, GetLiquidAmount()) / 1000);
}


// 1000 liquid items count as 1 wealth unit
// this may have to be tuned or made object-specific?
func CalcValue(object in_base, int for_plr)
{
	return GetID()->GetValue() * Max(1, GetLiquidAmount()) / 1000;
}


// -------------- Interaction
//
// Interfaces for interaction with other objects


/** 
Inserts liquid into the object.
@param liquid_name: Material to insert
@param amount: Max Amount of Material being inserted 
@param source: Object which inserts the liquid
@return returned_amount: The inserted amount
*/
func PutLiquid(string liquid_name, int amount, object source)
{
	if (amount < 0)
	{
		FatalError(Format("You can insert positive amounts of liquid only, got %d", amount));
	}

	if (liquid_name == GetLiquidType())
	{
		amount = BoundBy(MaxStackCount() - GetLiquidAmount(), 0, amount);
		DoStackCount(+amount);
		return amount;
	}
	else //Wrong material?
	{
		return 0;
	}
}


/**
Extracts liquid from the object.
@param liquid_name: Material to extract; Wildcardsupport
               Defaults to the current liquid if 'nil' is passed.
@param amount: Max Amount of liquid being extracted;
               Defaults to all contained liquid if 'nil' is passed.
@param destination: Object that extracts the liquid
@return [returned_liquid, returned_amount]
	   - returned_liquid: Material being extracted
	   - returned_amount: Amount being extracted
*/
func RemoveLiquid(string liquid_name, int amount, object destination)
{
	if (amount < 0)
	{
		FatalError(Format("You can remove positive amounts of liquid only, got %d", amount));
	}

	// default parameters if nothing is provided: the current material and level
	liquid_name = liquid_name ?? GetLiquidType();
	amount = amount ?? GetLiquidAmount();

	//Wrong material?
	if (!WildcardMatch(GetLiquidType(), liquid_name))
		return [GetLiquidType(), 0];

	amount = Min(amount, GetLiquidAmount());
	DoStackCount(-amount);
	return [liquid_name, amount];
}


/**
 Converts a liquid name to a definition
 that represents that liquid.
 @par liquid_name the name of the liquid
 @return the Id of the liquid object, 
         or nil if no such object exists
 */
func GetLiquidID(string liquid_name)
{
	if (liquid_name == "Acid") return Acid;
	if (liquid_name == "Lava") return Lava;
	if (liquid_name == "Oil") return Oil;
	if (liquid_name == "Water") return Water;
	return Library_Liquid;	
}


/**
 Creates a liquid object with the specified name
 and amount. Liquids with amount 0 can be created
 that way.
 */
func CreateLiquid(string liquid_name, int amount)
{
	var item = CreateObject(GetLiquidID(liquid_name));
	item->SetStackCount(amount);
	return item;
}

public func CanBeStackedWith(object other)
{
	var is_same_liquid = other->~GetLiquidType() == this->~GetLiquidType();
	
	return _inherited(other, ...) && is_same_liquid;
}

protected func RejectEntrance(object into)
{
	if (_inherited(into, ...)) return true;
	if (into->GetAlive()) return true;
	return !(into->~IsLiquidContainer() || into->~IsContainer());
}
