
void mrInstanceFluid::write()
{
   if ( ! options->exportFluids ) return;

   mrFluid* fluid = static_cast< mrFluid* >( shape );
   fluid->write_fluid_data();
   mrInstanceObject::write();
}
