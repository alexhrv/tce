
-- File       : fpmac.vhdl
-- Author     : Timo Viitanen  <timo.2.viitanen@tut.fi>
-- Company    :
-- Created    : 2011/04/25
-- Description: Multiply-accumulate block for floating point unit
--      Arithmetic code is based on IEEE floating point VHDL standard
--              libraries (floatfixlib in Modelsim)

library IEEE;
use IEEE.std_logic_1164.all;
LIBRARY WORK;

package float_fma32_opcodes is
  constant OPC_ADDF       : std_logic_vector(2 downto 0) := "000";
  constant OPC_MACF       : std_logic_vector(2 downto 0) := "001";
  constant OPC_MSUF       : std_logic_vector(2 downto 0) := "010";
  constant OPC_MULF       : std_logic_vector(2 downto 0) := "011";
  constant OPC_SUBF       : std_logic_vector(2 downto 0) := "100";
-- TODO squaref?
end float_fma32_opcodes;


library IEEE;
use IEEE.STD_LOGIC_1164.all;
use IEEE.NUMERIC_STD.all;

LIBRARY WORK;
use work.fixed_pkg.all;
use work.float_pkg.all;
use work.fixed_float_types.all;
use work.float_fma32_opcodes.all;


-- r1 = o1 + t1*o2

ENTITY fpfma32 IS
  GENERIC(
    dataw : integer := 32;
    busw : integer := 32;
    ew    : integer := 8;
    mw   : integer := 23;
    
-- Generic parameters for adjusting latency. 
-- F. ex. bypass_2 turns pipeline stage 2 registers into wires.
-- Based on some benchmarking with a high-end stratix 2, 
-- the fastest combinations for each latency seem to be:
-- Latency 5: bypass_2
-- Latency 4: bypass_2 bypass_4
-- Default setting is latency 6.

    bypass_2   : boolean := False;
    bypass_3   : boolean := False;
    bypass_4   : boolean := False;
    bypass_5   : boolean := False
    );
  PORT (
    clk      : IN std_logic;
    rstx     : IN std_logic;
    glock    : IN std_logic;
    t1load   : IN std_logic;
    t1opcode : IN std_logic_vector(2 DOWNTO 0);
    o1load   : IN std_logic;
    o2load   : IN std_logic;
    t1data   : IN  std_logic_vector(dataw-1 DOWNTO 0);  
    o1data   : IN  std_logic_vector(dataw-1 DOWNTO 0);
    o2data   : IN  std_logic_vector(dataw-1 DOWNTO 0);
    r1data   : OUT std_logic_vector(busw-1 DOWNTO 0)

    );

END fpfma32;


ARCHITECTURE rtl OF fpfma32 IS

  CONSTANT guard_bits : INTEGER := 3; --guard bits for extra
                                                     --precision
  COMPONENT fpfma32_block
    GENERIC (
      exp_w      : integer := float_exponent_width;
      frac_w     : integer := float_fraction_width;
      guard      : integer := guard_bits;
      bypass_2   : boolean := False;
      bypass_3   : boolean := False;
      bypass_4   : boolean := False);

    PORT (
      m1_in           : IN  float(ew DOWNTO -mw);
      m2_in           : IN  float(ew DOWNTO -mw);
      a_in           : IN  float(ew DOWNTO -mw);
      sign        : OUT std_ulogic;
      round_guard : OUT std_ulogic;
      exp_out     : OUT signed(ew+1 DOWNTO 0);
      frac_out    : OUT unsigned(2*mw+2 downto 0);

      t1load_delay1 : in std_logic;
      t1load_delay4 : out std_logic;

      clk      : IN std_logic;
      rstx     : IN std_logic;
      glock    : IN std_logic
      );
  END COMPONENT;
  
  
  function vec2str(vec: std_logic_vector) return string is 
  variable res: string(vec'left + 1 downto 1); 
  begin 
    for i in vec'reverse_range loop 
      if (vec(i) = '1') then 
	res(i + 1) := '1'; 
      elsif (vec(i) = '0') then 
	res(i + 1) := '0'; 
      else 
	res(i + 1) := 'X'; 
      end if; 
    end loop; 
  return res; 
  end function vec2str;
  SIGNAL mac_in_m1  : float(ew DOWNTO -mw);
  SIGNAL mac_in_m2  : float(ew DOWNTO -mw);
  SIGNAL mac_in_a   : float(ew DOWNTO -mw);
  SIGNAL o1tempdata : std_logic_vector(ew + mw downto 0);
  SIGNAL o2tempdata : std_logic_vector(ew + mw downto 0);
  SIGNAL add_out    : float(ew DOWNTO -mw);
  
  SIGNAL sign_out_add : std_ulogic;
  SIGNAL round_guard_out_add : std_ulogic;
  SIGNAL exp_out_add : signed(ew+1 DOWNTO 0);
  SIGNAL frac_out_add : unsigned(2*mw+2 downto 0);
  SIGNAL sign_norm : std_ulogic;
  SIGNAL round_guard_norm : std_ulogic;
  SIGNAL exp_in_norm : signed(ew+1 DOWNTO 0);
  SIGNAL frac_in_norm : unsigned(2*mw+2 downto 0);
  SIGNAL res_out_norm : float(ew DOWNTO -mw);

  signal t1load_delay1_r : std_logic;
  signal t1load_delay4 : std_logic;
  signal t1load_delay5_r : std_logic;
  
  constant one : std_logic_vector(ew+mw downto 0) := X"3f800000";
  constant zero : std_logic_vector(ew+mw downto 0) := X"80000000";

  -- Truncated t1data, o1data, r1data with shorter word lengths 
  signal t1trun : std_logic_vector(ew + mw downto 0);
  signal o1trun : std_logic_vector(ew + mw downto 0);
  signal o2trun : std_logic_vector(ew + mw downto 0);
  signal o3trun : std_logic_vector(ew + mw downto 0);
  signal r1trun : std_logic_vector(ew + mw downto 0);

BEGIN  
 
  macer : fpfma32_block
    GENERIC MAP(
      exp_w => ew,
      frac_w => mw,
      guard => guard_bits,
      bypass_2 => bypass_2,
      bypass_3 => bypass_3,
      bypass_4 => bypass_4)
    PORT MAP (
      m1_in        => mac_in_m1,
      m2_in        => mac_in_m2,
      a_in        => mac_in_a,
      sign     => sign_out_add,
      round_guard => round_guard_out_add,
      exp_out  => exp_out_add,
      frac_out => frac_out_add,

      t1load_delay1 => t1load_delay1_r,
      t1load_delay4 => t1load_delay4,

      clk => clk,
      rstx => rstx,
      glock => glock );
      
  -- Must use internally smaller word length
  t1trun <= t1data(ew + mw downto 0);
  o1trun <= o1data(ew + mw downto 0);
  o2trun <= o2data(ew + mw downto 0);
  r1data(ew + mw downto 0) <= r1trun;
  --r1data(busw-1 downto ew + mw + 1) <= (others => '0');

  fpu: PROCESS (clk, rstx, sign_out_add,round_guard_out_add,exp_out_add,frac_out_add)
  BEGIN  -- PROCESS fpu

    IF(rstx = '0') THEN  
      --r1trun <= (OTHERS => '0');
      mac_in_m1 <= (OTHERS => '0');
      mac_in_m2 <= (OTHERS => '0');
      mac_in_a <= (OTHERS => '0');
      o1tempdata <= (OTHERS => '0');
      
      IF bypass_5=False THEN
        t1load_delay5_r <= '0';
        sign_norm <= '0';
        round_guard_norm <= '0';
        exp_in_norm <= (others => '0');
        frac_in_norm <= (others => '0');
      END IF;

    ELSIF(clk'event AND clk = '1') then
      if(glock = '0') then

        t1load_delay1_r <= t1load;

        if( t1load = '1' ) then
        
          if ( t1opcode = OPC_SUBF ) or ( t1opcode = OPC_MSUF ) then
            mac_in_m1 <= to_float( (not t1trun(mw+ew)) & t1trun(mw+ew-1 downto 0), ew, mw );
          else
            mac_in_m1 <= to_float( t1trun, ew, mw );
          end if;

          if( ( t1opcode = OPC_ADDF ) or ( t1opcode = OPC_SUBF ) ) then
            mac_in_m2 <= to_float( one, ew, mw );
          else
            if( o2load = '1' ) then
              mac_in_m2 <= to_float(o2trun, ew, mw);
            else
              mac_in_m2 <= to_float(o2tempdata, ew, mw);
            end if;
          end if;

          if( t1opcode = OPC_MULF ) then
            mac_in_a <= to_float( zero, ew, mw );
          else
            if( o1load = '1' ) then
                mac_in_a <= to_float(o1trun, ew, mw);
            else
                mac_in_a <= to_float(o1tempdata, ew, mw);
            end if;
          end if;
        end if;

        if( o1load = '1' ) then
            o1tempdata <= o1trun;
        end if;

        if( o2load = '1' ) then
            o2tempdata <= o2trun;
        end if;
        
        IF bypass_5=False and t1load_delay4='1' THEN
          t1load_delay5_r <= t1load_delay4;
          sign_norm <= sign_out_add;
          round_guard_norm <= round_guard_out_add;
          exp_in_norm <= to_signed(-21, ew+2) + exp_out_add;
          frac_in_norm <= frac_out_add;
        END IF;
        
        if t1load_delay5_r = '1' then
          r1trun <= to_slv(res_out_norm);
        end if;
        
      END IF;
    END IF;    

    IF bypass_5=True THEN
      t1load_delay5_r <= t1load_delay4;
      sign_norm <= sign_out_add;
      round_guard_norm <= round_guard_out_add;
      exp_in_norm <= to_signed(-21, ew+2) + exp_out_add;
      frac_in_norm <= frac_out_add;
    END IF;    
  END PROCESS fpu;
  
  res_out_norm <= normalize (fract  => frac_in_norm,
                          expon  => exp_in_norm,
                          sign   => sign_norm,
                          sticky => round_guard_norm,
                          exponent_width => ew,
                          fraction_width => mw,
                        --  round_style    => round_zero,
                          round_style    => round_nearest,
                          denormalize    => false,
                          nguard         => guard_bits);
END rtl;


-- File       : fpadd_block.vhdl
-- Author     : Timo Viitanen  <timo.2.viitanen.fi>
-- Company    :
-- Created    : 2011/04/25
-- Description: Multiply-accumulate block for floating point unit
--      Arithmetic code is based on IEEE floating point VHDL standard
--              libraries (floatfixlib in Modelsim)

library IEEE;
use IEEE.std_logic_1164.all;
use ieee.numeric_std.all;
use work.fixed_pkg.all;
use work.float_pkg.all;
use work.fixed_float_types.all;


-- a + m1*m2
ENTITY fpfma32_block IS
  GENERIC (
    exp_w      : integer := float_exponent_width;
    frac_w     : integer := float_fraction_width;
    guard      : integer := float_guard_bits;
    bypass_2   : boolean := False;
    bypass_3   : boolean := False;
    bypass_4   : boolean := False);
  PORT (

    a_in       : IN  float(exp_w DOWNTO -frac_w);
    m1_in       : IN  float(exp_w DOWNTO -frac_w);
    m2_in       : IN  float(exp_w DOWNTO -frac_w);
    sign : OUT std_ulogic;
    round_guard : OUT std_ulogic;
    exp_out : OUT signed(exp_w+1 DOWNTO 0);
    frac_out : OUT unsigned((2*frac_w)+2 downto 0);

    t1load_delay1 : in std_logic;
    t1load_delay4 : out std_logic;

    clk      : IN std_logic;
    rstx     : IN std_logic;
    glock    : IN std_logic
    );
END fpfma32_block;

ARCHITECTURE rtl OF fpfma32_block IS
  signal stage1_fractl, stage2_fractl : UNSIGNED (frac_w downto 0);
  signal stage1_fractr, stage2_fractr : UNSIGNED (frac_w downto 0);
  signal stage2_exponl, stage1_exponl : SIGNED (exp_w-1 downto 0);
  signal stage2_exponr, stage1_exponr : SIGNED (exp_w-1 downto 0);
  signal stage3_fractx, stage2_fractx : UNSIGNED (frac_w+guard downto 0);
  signal stage3_exponc, stage2_exponc : SIGNED (exp_w-1 downto 0);
  signal stage3_rfract, stage2_rfract : UNSIGNED ((2*(frac_w))+2 downto 0);
  signal stage3_rexpon, stage2_rexpon : SIGNED (exp_w+1 downto 0);
  signal stage3_shiftx, stage2_shiftx : SIGNED (exp_w+1 downto 0);  -- shift fractions
  signal stage4_fractc, stage3_fractc : UNSIGNED (2*frac_w+2 downto 0);
  signal stage4_fracts, stage3_fracts : UNSIGNED (2*frac_w+2 downto 0);

  signal stage2_lsign, stage3_lsign, stage4_lsign : std_ulogic;
  signal stage2_rsign, stage3_rsign, stage4_rsign : std_ulogic;
  signal stage2_csign, stage3_csign, stage4_csign : std_ulogic;

  signal stage3_round_guard_out, stage4_round_guard_out : std_ulogic;
  signal stage3_exp_out, stage4_exp_out : SIGNED (exp_w+1 downto 0);
  signal stage3_leftright, stage4_leftright : boolean;

  signal stage2_c : float(exp_w DOWNTO -frac_w);
  
  signal t1load_delay2_r, t1load_delay3_r, t1load_delay4_r : std_logic;
  

  procedure fp_mac_stage1 (
      l               : IN  float(exp_w DOWNTO -frac_w);
      r               : IN  float(exp_w DOWNTO -frac_w);
      signal fractl_out      : OUT UNSIGNED (frac_w downto 0);
      signal fractr_out      : OUT UNSIGNED (frac_w downto 0);  -- fractions
      signal exponl_out      : OUT SIGNED (exp_w-1 downto 0);
      signal exponr_out      : OUT SIGNED (exp_w-1 downto 0) -- exponents
  )
    is
    variable fractional_a,
             fractional_b              : UNSIGNED (frac_w downto 0);

    variable exponl, exponr            : SIGNED (exp_w-1 downto 0);

    variable fractc, fracts            : UNSIGNED (frac_w+1+guard downto 0);
  begin

    --NO SUPPORT FOR DENORMALIZED NUMBERS HERE
    --unpack input a
    if ( or_reduce(unsigned(to_slv(l(exp_w-1 downto 0)))) /= '0') then      
      fractional_a(frac_w-1 DOWNTO 0)  := UNSIGNED (to_slv(
        l(-1 downto -frac_w)));
      fractional_a(frac_w) := '1';
      exponl := SIGNED(l(exp_w-1 DOWNTO 0));
      exponl(exp_w-1) := NOT exponl(exp_w-1);
    else

      fractional_a := (others => '0');
      exponl := (others => '1');
      exponl(exp_w-1) := '0';
      exponl := -exponl;
    end if;
    --unpack input b
    if(or_reduce(unsigned(to_slv(r(exp_w-1 downto 0)))) /= '0') then
      fractional_b(frac_w-1 DOWNTO 0)  := UNSIGNED (to_slv(
        r(-1 downto -frac_w)));
      fractional_b(frac_w) := '1';
      exponr := SIGNED(r(exp_w-1 DOWNTO 0));
      exponr(exp_w-1) := NOT exponr(exp_w-1);
    else
      fractional_b := (others => '0');
      exponr := (others => '1');
      exponr(exp_w-1) := '0';
      exponr := -exponr;
    end if;

    fractl_out <= (OTHERS => '0');
    fractr_out <= (OTHERS => '0');
    fractl_out(frac_w DOWNTO 0) <= fractional_a;
    fractr_out(frac_w DOWNTO 0) <= fractional_b;
    exponl_out <= exponl;
    exponr_out <= exponr;
    
  end procedure fp_mac_stage1;
  
  
  procedure fp_mac_stage2 (
      c               : IN  float(exp_w DOWNTO -frac_w);
      fractl          : IN UNSIGNED (frac_w downto 0);
      fractr          : IN UNSIGNED (frac_w downto 0);  -- fractions
      exponl          : IN SIGNED (exp_w-1 downto 0);
      exponr          : IN SIGNED (exp_w-1 downto 0); -- exponents
      signal rfract_out          : OUT UNSIGNED (2*frac_w+2 downto 0);
      signal rexpon_out          : OUT SIGNED (exp_w+1 downto 0);
      signal fractx_out          : OUT UNSIGNED (frac_w+guard downto 0);
      signal exponc_out          : OUT SIGNED (exp_w-1 downto 0);
      signal shiftx_out          : OUT SIGNED (exp_w+1 downto 0)  -- shift fractions
  )
  is
    variable fractional_c              : UNSIGNED (frac_w downto 0);
    variable fractx                    : UNSIGNED (frac_w+guard downto 0);
    variable exponc                    : SIGNED (exp_w-1 downto 0); 
    variable shiftx                    : SIGNED (exp_w+1 downto 0);  -- shift fractions
    variable rexpon                    : SIGNED (exp_w+1 downto 0);
  begin
  
    --unpack input c
    if(or_reduce(unsigned(to_slv(c(exp_w-1 downto 0)))) /= '0') then
      fractional_c(frac_w-1 DOWNTO 0)  := UNSIGNED (to_slv(
        c(-1 downto -frac_w)));
      fractional_c(frac_w) := '1';
      exponc := SIGNED(c(exp_w-1 DOWNTO 0));
      exponc(exp_w-1) := NOT exponc(exp_w-1);
    else
      fractional_c := (others => '0');
      exponc := (others => '1');
      exponc(exp_w-1) := '0';
      exponc := -exponc;
      exponc := to_signed(-128,exponc'length);
    end if;
    fractx := (OTHERS => '0');
    fractx(frac_w+guard DOWNTO guard) := fractional_c;
    
    fractx_out <= fractx;
    exponc_out <= exponc;
    
    rexpon := resize (exponl, rexpon_out'length) + exponr + 1;
    
    shiftx := rexpon - exponc;
    
    --if exponc = -15 then
    --  shiftx := to_signed(32,shiftx'length);
    --end if;
    
    shiftx_out <= shiftx;

    -- multiply
    rfract_out <= (fractl * fractr) & '0';
    rexpon_out <= rexpon;
  end procedure fp_mac_stage2;
  
  function vec2str(vec: std_logic_vector) return string is 
  variable res: string(vec'left + 1 downto 1); 
  begin 
    for i in vec'reverse_range loop 
      if (vec(i) = '1') then 
	res(i + 1) := '1'; 
      elsif (vec(i) = '0') then 
	res(i + 1) := '0'; 
      else 
	res(i + 1) := 'X'; 
      end if; 
    end loop; 
  return res; 
  end function vec2str;
  
  procedure fp_mac_stage3 (
      rfract          : IN UNSIGNED (2*frac_w+2 downto 0);
      rexpon          : IN SIGNED (exp_w+1 downto 0);
      fractx          : IN UNSIGNED (frac_w+guard downto 0);
      exponc          : IN SIGNED (exp_w-1 downto 0);  -- fractions
      shiftx          : IN SIGNED (exp_w+1 downto 0);  -- shift fractions

      signal fractc_out          : OUT UNSIGNED (2*frac_w+2 downto 0);
      signal fracts_out          : OUT UNSIGNED (2*frac_w+2 downto 0);
      signal leftright_out          : OUT boolean;

      signal round_guard_out : OUT std_ulogic;
      signal exp_out         : OUT signed(exp_w+1 DOWNTO 0)
  )
  is
    variable rfract_var                : UNSIGNED (2*frac_w+2 downto 0);
    variable rexpon_var                : SIGNED (exp_w+1 downto 0);
    variable fractx_tmp                : UNSIGNED (2*frac_w+2 downto 0);
    variable exponc_tmp                : SIGNED (exp_w-1 downto 0);

    variable fractc, fracts            : UNSIGNED (2*frac_w+2 downto 0);
    variable fractstmp                 : UNSIGNED (2*frac_w+2 downto 0);
    variable fractlt                   : BOOLEAN;     -- 
    variable exponeq                   : BOOLEAN;     -- 
    variable exponlt                   : BOOLEAN;     -- 
    variable overflow                  : BOOLEAN;     -- 
    variable leftright                 : BOOLEAN;     -- left or right used
    variable sticky                    : STD_ULOGIC;  -- Holds precision for rounding
    variable rexpon2                   : SIGNED (exp_w+1 downto 0);  -- result exponent
    variable shiftx_var                : SIGNED (rexpon'range);  -- shift fractions
    
    --constant shiftdataw : integer := 2*frac_w+2;
    constant max_shift : integer := rexpon'high-1;
    constant shiftdataw : integer := 2**max_shift-1;
    type shift_array is array (natural range <>) of unsigned(shiftdataw-1 downto 0);
    variable y_temp : shift_array (0 to max_shift);
  begin
    
    rfract_var := rfract;
    rexpon_var := rexpon;
    shiftx_var := shiftx;
  
    fractx_tmp := (others=>'0');
    fractx_tmp(fractx_tmp'high-1 downto fractx_tmp'high - (frac_w+guard+1)) := fractx;
    exponc_tmp := exponc;
    
    if rfract_var( rfract_var'high ) = '1' then
      rfract_var := '0' & rfract_var( rfract_var'high downto 1 );
      rexpon_var := rexpon_var + 1;
      shiftx_var := shiftx_var + 1;
    end if;
  
    exponeq  := shiftx_var = 0;
    exponlt  := shiftx_var < 0;
    fractlt  := rfract_var < fractx_tmp;
    leftright := not( exponlt or (exponeq and fractlt) );
    
    if exponc_tmp = -128 then 
      exponeq := false;
      exponlt := false;
      leftright := true;
      shiftx_var := (others=>'0');
    end if;
    
    if leftright then
      rexpon2   := rexpon_var;
      fractc    := rfract_var;--rfract (rfract'high downto rfract'high - fractc'length+1);
      fractstmp := fractx_tmp;
    else
      rexpon2   := resize (exponc_tmp, rexpon2'length);
      fractc    := fractx_tmp;
      fractstmp := rfract_var;--rfract (rfract'high downto rfract'high - fractc'length+1);
    end if;
    
    if exponlt then
      shiftx_var := - shiftx_var;
    end if;
    
    overflow := shiftx_var > fractx'high;
    
    sticky := '0';
    y_temp(0) := (others=>'0');
    y_temp(0)(fractstmp'range) := fractstmp;
    for i in 0 to max_shift-1 loop
      if (shiftx_var(i) = '1') then
        y_temp(i+1) := (others => '0');
        y_temp(i+1) (shiftdataw-1-2**i downto 0) := y_temp(i) (shiftdataw-1 downto 2**i);
        if y_temp(i) ((2**i)-1 downto 0) /= to_unsigned(0,2**i) then
          sticky := '1';
        end if;
      else
        y_temp(i+1) := y_temp(i);
      end if;
    end loop;  -- i

    if overflow then
      fracts  := (others => '0');
      if fractstmp /= 0 then
        sticky := '1';
      end if;
    else 
      fracts := y_temp(y_temp'high)(fracts'range);
      --fracts := shift_right (fractstmp, to_integer(shiftx_var));
    end if;
    
    fracts(0) := fracts(0) or sticky;

    fractc_out <= fractc;
    fracts_out <= fracts;
    round_guard_out <= sticky;
    exp_out <= rexpon2;
    leftright_out <= leftright;
    

  end procedure fp_mac_stage3;
  
  
  procedure fp_mac_stage4 (
      fractc          : IN UNSIGNED (2*frac_w+2 downto 0);
      fracts          : IN UNSIGNED (2*frac_w+2 downto 0);
      lsign           : IN std_ulogic;
      rsign           : IN std_ulogic;
      csign           : IN std_ulogic;
      leftright       : in boolean;

      signal sign_out        : OUT std_ulogic;
      signal frac_out        : OUT unsigned(2*frac_w+2 downto 0)
  )
  is
    variable fp_sign                   : STD_ULOGIC;  -- sign of result
    variable ufract                    : UNSIGNED (2*frac_w+2 downto 0);  -- result fraction
  begin

    
    fp_sign := lsign xor rsign;
    if fp_sign = to_X01(csign) then
      ufract := fractc + fracts;
      fp_sign := fp_sign;
    else                              -- signs are different
      ufract := fractc - fracts;      -- always positive result
      
      if fractc = fracts then -- Figure out which sign to use
        fp_sign := '0';
      elsif leftright or unsigned(fractc)=0 then 
        fp_sign := fp_sign;
      else
        fp_sign := csign;
      end if;
    end if;
    
    frac_out <= ufract; --ufract(ufract'high downto 1);
    sign_out <= fp_sign;
      
  end procedure fp_mac_stage4;
  
BEGIN  -- rtl

  fp_mac_stage1(
    l      =>      m1_in,
    r      =>      m2_in,
    fractl_out =>      stage1_fractl,
    fractr_out =>      stage1_fractr,
    exponl_out =>      stage1_exponl,
    exponr_out =>      stage1_exponr );

  fp_mac_stage2(
    c      =>      stage2_c,
    fractl =>      stage2_fractl,
    fractr =>      stage2_fractr,
    exponl =>      stage2_exponl,
    exponr =>      stage2_exponr,
    rfract_out =>      stage2_rfract,
    rexpon_out =>      stage2_rexpon,
    fractx_out =>      stage2_fractx,
    exponc_out =>      stage2_exponc,
    shiftx_out =>      stage2_shiftx);

  fp_mac_stage3(
    rfract =>      stage3_rfract,
    rexpon =>      stage3_rexpon,
    fractx =>      stage3_fractx,
    exponc =>      stage3_exponc,
    shiftx =>      stage3_shiftx,
    fractc_out =>      stage3_fractc,
    fracts_out =>      stage3_fracts,
    leftright_out =>   stage3_leftright,
    round_guard_out => stage3_round_guard_out,
    exp_out         => stage3_exp_out);

  fp_mac_stage4(
    fractc =>      stage4_fractc,
    fracts =>      stage4_fracts,
    lsign  =>      stage4_lsign,
    rsign  =>      stage4_rsign,
    csign  =>      stage4_csign,
    leftright =>   stage4_leftright,
    sign_out =>    sign,
    frac_out =>    frac_out);
    
  stage2_wires : if bypass_2 = True generate
    wires : process(stage1_fractl,stage1_fractr,stage1_exponl,stage1_exponr,m1_in,m2_in,a_in, t1load_delay1) is
    begin
      t1load_delay2_r <= t1load_delay1;

      stage2_fractl <= stage1_fractl;
      stage2_fractr <= stage1_fractr;
      stage2_exponl <= stage1_exponl;
      stage2_exponr <= stage1_exponr;
        
      stage2_lsign  <= m1_in(m1_in'high);
      stage2_rsign  <= m2_in(m2_in'high);
      stage2_csign  <= a_in(a_in'high);
        
      stage2_c      <= a_in;
    end process wires;
  end generate;
  
  stage2_regs : if bypass_2 = False generate
    regs: PROCESS (clk, rstx)
    BEGIN  -- PROCESS fpu
      IF(rstx = '0') THEN  
        t1load_delay2_r <= '0';

        stage2_fractl <= (others=>'0');
        stage2_fractr <= (others=>'0');
        stage2_exponl <= (others=>'0');
        stage2_exponr <= (others=>'0');
          
        stage2_lsign  <= '0';
        stage2_rsign  <= '0';
        stage2_csign  <= '0';
          
        stage2_c      <= (others=>'0');
      ELSIF(clk'event AND clk = '1') then
        IF(glock = '0') then
          t1load_delay2_r <= t1load_delay1;

          if t1load_delay1 = '1' then
            stage2_fractl <= stage1_fractl;
            stage2_fractr <= stage1_fractr;
            stage2_exponl <= stage1_exponl;
            stage2_exponr <= stage1_exponr;
              
            stage2_lsign  <= m1_in(m1_in'high);
            stage2_rsign  <= m2_in(m2_in'high);
            stage2_csign  <= a_in(a_in'high);
              
            stage2_c      <= a_in;
          end if;
        END IF;
      END IF;
    END PROCESS regs;
  end generate;
    
  stage3_wires : if bypass_3 = True generate
    wires : process(stage2_rfract,stage2_rexpon,stage2_fractx,stage2_exponc,stage2_shiftx,
                    stage2_lsign, stage2_rsign, stage2_csign, t1load_delay2_r) is
    begin
      t1load_delay3_r <= t1load_delay2_r;

      stage3_rfract <= stage2_rfract;
      stage3_rexpon <= stage2_rexpon;
      stage3_fractx <= stage2_fractx;
      stage3_exponc <= stage2_exponc;
      stage3_shiftx <= stage2_shiftx;
      
      stage3_lsign  <= stage2_lsign;
      stage3_rsign  <= stage2_rsign;
      stage3_csign  <= stage2_csign;
    end process wires;
  end generate;
  
  stage3_regs : if bypass_3 = False generate
    regs: PROCESS (clk, rstx)
    BEGIN  -- PROCESS fpu
      IF(rstx = '0') THEN  
        t1load_delay3_r <= '0';

        stage3_rfract <= (others=>'0');
        stage3_rexpon <= (others=>'0');
  
        stage3_lsign  <= '0';
        stage3_rsign  <= '0';
        stage3_csign  <= '0';
      ELSIF(clk'event AND clk = '1') then
        IF(glock = '0') then
          t1load_delay3_r <= t1load_delay2_r;

          if t1load_delay2_r = '1' then
            stage3_rfract <= stage2_rfract;
            stage3_rexpon <= stage2_rexpon;
            stage3_fractx <= stage2_fractx;
            stage3_exponc <= stage2_exponc;
            stage3_shiftx <= stage2_shiftx;
            
            stage3_lsign  <= stage2_lsign;
            stage3_rsign  <= stage2_rsign;
            stage3_csign  <= stage2_csign;
          end if;
        END IF;
      END IF;
    END PROCESS regs;
  end generate;
  
  stage4_wires : if bypass_4 = True generate
    wires : process(stage3_fractc, stage3_fracts, stage3_leftright,
                    stage3_lsign, stage3_rsign, stage3_csign,
                    stage3_round_guard_out, stage3_exp_out, t1load_delay3_r) is
    begin
      t1load_delay4_r <= t1load_delay3_r;

      stage4_fractc <= stage3_fractc;
      stage4_fracts <= stage3_fracts;
      stage4_leftright <= stage3_leftright;
      
      stage4_lsign  <= stage3_lsign;
      stage4_rsign  <= stage3_rsign;
      stage4_csign  <= stage3_csign;

      stage4_round_guard_out <= stage3_round_guard_out;
      stage4_exp_out         <= stage3_exp_out;
    end process wires;
  end generate;
  
  stage4_regs : if bypass_4 = False generate
    regs: PROCESS (clk, rstx)
    BEGIN  -- PROCESS fpu
      IF(rstx = '0') THEN  
        t1load_delay4_r <= '0';

        stage4_fractc <= (others=>'0');
        stage4_fracts <= (others=>'0');
        stage4_leftright <= False;
        
        stage4_lsign  <= '0';
        stage4_rsign  <= '0';
        stage4_csign  <= '0';

        stage4_round_guard_out <= '0';
        stage4_exp_out         <= (others=>'0');
      ELSIF(clk'event AND clk = '1') then
        IF(glock = '0') then
          t1load_delay4_r <= t1load_delay3_r;

          if t1load_delay3_r = '1' then
            stage4_fractc <= stage3_fractc;
            stage4_fracts <= stage3_fracts;
            stage4_leftright <= stage3_leftright;
            
            stage4_lsign  <= stage3_lsign;
            stage4_rsign  <= stage3_rsign;
            stage4_csign  <= stage3_csign;

            stage4_round_guard_out <= stage3_round_guard_out;
            stage4_exp_out         <= stage3_exp_out;
          end if;
        END IF;
      END IF;
    END PROCESS regs;
  end generate;

  exp_out <= stage4_exp_out;
  round_guard <= stage4_round_guard_out;
  t1load_delay4 <= t1load_delay4_r;

END rtl;
